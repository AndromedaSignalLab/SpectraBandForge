/*
FFTUtil namespace declarations of ModPlug Player
Copyright (C) 2024 Volkan Orhan

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include <cstddef>
#include <algorithm>
//#include <complex>

namespace  AndromedaSignalLab::FFTUtil {
    size_t getOutputDataElementAmount(size_t inputDataElementAmount);
    template <class T> T getFrequencySpacingOld(size_t sampleRate, size_t fftPrecision);
    template <class T> T getFrequencySpacing(size_t sampleRate, size_t inputDataElementAmount);
    template <class T> void removeMean(T * signalData, const size_t dataCount);
    template <class T> T calculateOverlapRatio(T binLow, T binHigh, T bandLow, T bandHigh);
}

template<class T> T AndromedaSignalLab::FFTUtil::getFrequencySpacingOld(size_t sampleRate, size_t fftPrecision) {
    return T(sampleRate)/(fftPrecision-1);
}

template<class T> T AndromedaSignalLab::FFTUtil::getFrequencySpacing(size_t sampleRate, size_t inputDataElementAmount) {
    return T(sampleRate)/inputDataElementAmount;
}

template<class T>
void AndromedaSignalLab::FFTUtil::removeMean(T *signalData, const size_t dataCount) {
    T mean = 0;
    for(size_t i = 0; i < dataCount; ++i)
        mean += signalData[i];

    mean /= dataCount;

    for(size_t i = 0; i < dataCount; ++i)
        signalData[i] -= mean;
}

template<class T> T AndromedaSignalLab::FFTUtil::calculateOverlapRatio( T binLow, T binHigh, T bandLow, T bandHigh) {
    T overlapLow = std::max(binLow, bandLow);
    T overlapHigh = std::min(binHigh, bandHigh);

    T overlapWidth = overlapHigh - overlapLow;
    if (overlapWidth <= 0.0)
        return 0.0;

    T binWidth = binHigh - binLow;
    if (binWidth <= 0.0)
        return 0.0;

    return overlapWidth / binWidth;
}