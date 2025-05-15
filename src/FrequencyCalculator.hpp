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
#include <boost/numeric/interval.hpp>

using namespace boost::numeric;

class FrequencyCalculator {
public:
    template <class T> static inline T calculateExactMidBandFrequency(size_t b, T G, T fr, int x);
    template <class T> static inline T calculateLowerEdgeBandFrequency(size_t b, T G, T fm);
    template <class T> static inline T calculateUpperEdgeBandFrequency(size_t b, T G, T fm);
    template<class T> static inline T getG(OctaveBandBase base = OctaveBandBase::Base10);
    static inline interval<int> calculateXInterval(size_t b);
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
