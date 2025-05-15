/*
FrequencyCalculator class declarations and definitions of SpectraBandForge Project
Copyright (C) 2025 Volkan Orhan

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#pragma once
#include <cstddef>
#include <cmath>
#include "BandDefinitions.hpp"
#include "MathUtil.hpp"
#include "SearchUtil.hpp"
#include <boost/numeric/interval.hpp>

using namespace boost::numeric;

class FrequencyCalculator {
public:
    template <class T> static inline T calculateExactMidBandFrequency(size_t b, T G, T fr, int x);
    template <class T> static inline T calculateNominalFrequency(size_t b, T frequency);
    template <class T> static inline T calculateLowerEdgeBandFrequency(size_t b, T G, T fm);
    template <class T> static inline T calculateUpperEdgeBandFrequency(size_t b, T G, T fm);
    template<class T> static inline T getG(OctaveBandBase base = OctaveBandBase::Base10);
    static inline interval<int> calculateXInterval(size_t b);
private:
    //Gets nominal frequency for b = 1 and 3
    template<class T> static inline T getNominalFrequency(T frequency);
};

inline interval<int> FrequencyCalculator::calculateXInterval(size_t b) {
    int lower = 0, upper = 0;
    double currentExactMidBandFrequency;
    double G = getG<double>();

    for(int x = -500; x < 500; x++) {
        currentExactMidBandFrequency = calculateExactMidBandFrequency<double>(b, G, fRef, x);
        if(lower == 0 && currentExactMidBandFrequency >= 20)
            lower = x;
        if(currentExactMidBandFrequency <= 20000 && upper < currentExactMidBandFrequency)
            upper = x;
    }

    return interval<int>(lower, upper);
}

template <class T>
inline T FrequencyCalculator::calculateExactMidBandFrequency(size_t b, T G, T fr, int x) {
    T fm;

    if (b % 2 == 1) {
        // Formula (2) for odd b:   fm = fr * G^(x / b)
        fm = fr * std::pow(G, T(x) / T(b));
    } else {
        // Formula (3) for even b:  fm = fr * G^((2x + 1) / (2b))
        fm = fr * std::pow(G, T(2 * x + 1) / (T(2) * T(b)));
    }

    return fm;
}

template<class T> inline T FrequencyCalculator::calculateNominalFrequency(size_t b, T fm) {
    if(b == 1 || b == 3)
        return getNominalFrequency(fm);
    else if(b == 2) {
        return AndromedaDSP::MathUtil::roundBy(fm, 3);
    }
    int firstDigit = AndromedaDSP::MathUtil::firstDigit(fm);
    if(firstDigit == 0)
        return AndromedaDSP::MathUtil::roundBy(fm, 0);
    if(firstDigit<5){
        return AndromedaDSP::MathUtil::roundBy(fm, 3);
    } else
        return AndromedaDSP::MathUtil::roundBy(fm, 2);
}

template <class T> inline T FrequencyCalculator::calculateLowerEdgeBandFrequency(size_t b, T G, T fm){
    return pow(G, T(-1) / (T(2) * T(2*b))) * fm;
}

template <class T> inline T FrequencyCalculator::calculateUpperEdgeBandFrequency(size_t b, T G, T fm){
    return pow(G, T(1) / (T(2) * T(2*b))) * fm;
}

template<class T> inline T FrequencyCalculator::getG(OctaveBandBase base) {
    const T G2 = T(2);
    const double G10 = pow(T(10), T(3)/T(10));
    T G = (base == OctaveBandBase::Base10) ? G10 : G2;
    return G;
}

template<class T> inline T FrequencyCalculator::getNominalFrequency(T fm){
    const T nominalFrequencies[] = {1, 1.25, 1.63, 2, 2.5, 3.15, 4, 5, 6.3, 8, 10, 12.5, 16.3, 20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315,
                                    400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150,
                                    4000, 5000, 6300, 8000, 10000, 12500, 16000, 20000};
    int size = sizeof(nominalFrequencies)/sizeof(nominalFrequencies[0]);
    return SearchUtil<T>::findClosest(nominalFrequencies, size, fm);
}
