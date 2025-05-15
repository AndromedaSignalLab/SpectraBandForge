/*
BandFilter class declarations and definitions of SpectraBandForge Project
Copyright (C) 2020 Volkan Orhan

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#pragma once
#include <cmath>
#include <vector>
#include "BandDefinitions.hpp"
#include "FrequencyCalculator.hpp"

#define REAL 0
#define IMAG 1

template <class T>
class SpectrumAnalyzerBandDTO {
public:
    OctaveBand<T> bandInfo;
    inline void addMagnitude(T magnitude);
    inline void resetMagnitude();
    inline T getMagnitude() const;
private:
    T magnitude = 0;
    size_t sampleAmount = 0;
};

template<class T>
inline T SpectrumAnalyzerBandDTO<T>::getMagnitude() const{
    if(magnitude == 0)
        return 0;
    return magnitude/T(sampleAmount);
}

template<class T>
inline void SpectrumAnalyzerBandDTO<T>::addMagnitude(T magnitude) {
    this->magnitude += magnitude;
    sampleAmount++;
}

template<class T>
inline void SpectrumAnalyzerBandDTO<T>::resetMagnitude() {
    magnitude = 0;
    sampleAmount = 0;
}

template <class T>
class BandFilter
{
public:
    static inline std::vector<OctaveBand<T>> calculateOctaveBands(OctaveBandBase base, size_t nthOctave, T minFreq = 20, T maxFreq = 20000);
    /*
    template<class FFTDataType> static inline
    std::vector<SpectrumAnalyzerBandDTO<T>>
    calculateSpectrumAnalyzerBands(const std::vector<OctaveBand<T>> &octaveBands, FFTDataType *fftData, size_t frameAmount, size_t sampleRate);
    */
};

/*
template <typename T> T BandFilter<T>::calculateExactMidBandFrequency(size_t b, T G, T fr, size_t bandNumber){
    T fm;
    if(b%2 == 1) {
        fm = pow(G, (bandNumber-T(30))/b)*fr;
    }
    else {
        fm = pow(G, (T(2)*bandNumber-T(59))/(b*T(2)))*fr;
    }
    return fm;
}
*/

// Inline Method Definitions
/*
 * "ANSI S1.11: Specification for Octave, Half-Octave, and Third Octave Band Filter Sets" (PDF). Retrieved 7 March 2018.
 */
template<class T> inline std::vector<OctaveBand<T>> BandFilter<T>::calculateOctaveBands(OctaveBandBase base, size_t nthOctave, T minFreq, T maxFreq) {
    const T fr = T(1000); //reference frequency
    const T G2 = T(2);
    const double G10 = pow(T(10), T(3)/T(10));

    T fm; //exact midband frequency
    T fm10;
    size_t b = nthOctave; //bandwidth designator
    std::vector<OctaveBand<T>> octaveBands;
    T i = 0;
    T currentBandIndex = 0;
    T G = (base == OctaveBandBase::Base10) ? G10 : G2;

    while(true) {
        OctaveBand<T> octaveBand;
        fm = FrequencyCalculator::calculateExactMidBandFrequency(b, G, fr, i);
        fm10 = (base == OctaveBandBase::Base10) ? fm : FrequencyCalculator::calculateExactMidBandFrequency(b, G10, fr, i);
        octaveBand.bandNumber = currentBandIndex;
        octaveBand.base = base;
        octaveBand.midBandFrequency = fm;
        octaveBand.lowerEdgeBandFrequency = FrequencyCalculator::calculateLowerEdgeBandFrequency(b, G, fm);
        octaveBand.upperEdgeBandFrequency = FrequencyCalculator::calculateUpperEdgeBandFrequency(b, G, fm);
        if(octaveBand.upperEdgeBandFrequency < minFreq) {
            i += T(1);
            continue;
        }
        octaveBand.nominalMidBandFrequency = FrequencyCalculator::calculateNominalFrequency<T>(b, fm10);
        if(octaveBand.lowerEdgeBandFrequency > maxFreq)
            break;
        octaveBands.push_back(octaveBand);
        i += T(1);
        currentBandIndex += T(1);
    }

    return octaveBands;
}

/*
template<class T>
template<class FFTDataType>
std::vector<SpectrumAnalyzerBandDTO<T>>
BandFilter<T>::calculateSpectrumAnalyzerBands(const std::vector<OctaveBand<T>> &octaveBands, FFTDataType *fftData,
                                              size_t frameAmount, size_t sampleRate) {
    std::vector<SpectrumAnalyzerBandDTO<T>> spectrumAnalyzerBands(octaveBands.size());
    for(OctaveBand<T> &octaveBand : octaveBands) {
        SpectrumAnalyzerBandDTO<T> spectrumAnalyzerBand;
        spectrumAnalyzerBand.bandInfo = octaveBand;
        spectrumAnalyzerBands.push_back(spectrumAnalyzerBand);
    }
    return spectrumAnalyzerBands;
}*/

template <typename T>
class SpectrumAnalyzerBands {
    public:
        SpectrumAnalyzerBands();
        SpectrumAnalyzerBands(const std::vector<OctaveBand<T>> &octaveBands);
        SpectrumAnalyzerBandDTO<T> & operator [](const T frequency);
        void resetMagnitudes();
        std::vector<SpectrumAnalyzerBandDTO<T>> getData();
        void getData(SpectrumAnalyzerBandDTO<T> * bandData);
        void getAmplitudes(T * amplitudes);
        void getAmplitudes(T * amplitudes, size_t beginningIndex);
        void getAmplitudes(T * amplitudes, size_t beginningIndex, size_t endingIndex);
    private:
        void init();
        std::vector<SpectrumAnalyzerBandDTO<T>> spectrumAnalyzerBands;
        SpectrumAnalyzerBandDTO<T> outsideBand;
};

template<typename T>
SpectrumAnalyzerBandDTO<T> &SpectrumAnalyzerBands<T>::operator[](const T frequency) {
    for(SpectrumAnalyzerBandDTO<T> &spectrumAnalyzerBand:spectrumAnalyzerBands){
        if(frequency >= spectrumAnalyzerBand.bandInfo.lowerEdgeBandFrequency && frequency < spectrumAnalyzerBand.bandInfo.upperEdgeBandFrequency)
            return spectrumAnalyzerBand;
    }
    SpectrumAnalyzerBandDTO<T> & lastBand = spectrumAnalyzerBands[spectrumAnalyzerBands.size()-1];
    if(frequency == lastBand.bandInfo.upperEdgeBandFrequency)
        return lastBand;
    return outsideBand;
}

template<typename T>
void SpectrumAnalyzerBands<T>::init() {
    outsideBand.bandInfo.midBandFrequency = -1;
    outsideBand.bandInfo.nominalMidBandFrequency = -1;
    outsideBand.bandInfo.upperEdgeBandFrequency = -1;
    outsideBand.bandInfo.lowerEdgeBandFrequency = -1;
}

template<typename T>
SpectrumAnalyzerBands<T>::SpectrumAnalyzerBands(const std::vector<OctaveBand<T> > &octaveBands) {
    init();
    spectrumAnalyzerBands.clear();
    spectrumAnalyzerBands.reserve(octaveBands.size());
    for(const OctaveBand<T> &octaveBand : octaveBands) {
        SpectrumAnalyzerBandDTO<T> spectrumAnalyzerBand;
        spectrumAnalyzerBand.bandInfo = octaveBand;
        spectrumAnalyzerBands.push_back(spectrumAnalyzerBand);
    }
}

template<typename T>
SpectrumAnalyzerBands<T>::SpectrumAnalyzerBands() {
    init();
}

template<typename T>
void SpectrumAnalyzerBands<T>::resetMagnitudes() {
    for(SpectrumAnalyzerBandDTO<T> &spectrumAnalyzerBandDto : spectrumAnalyzerBands) {
        spectrumAnalyzerBandDto.resetMagnitude();
    }
}

template<typename T>
std::vector<SpectrumAnalyzerBandDTO<T>> SpectrumAnalyzerBands<T>::getData() {
    return spectrumAnalyzerBands;
}

template<typename T>
void SpectrumAnalyzerBands<T>::getData(SpectrumAnalyzerBandDTO<T> *bandData) {
    for(int i=0; i<spectrumAnalyzerBands.size(); i++) {
        bandData[i] = spectrumAnalyzerBands[i];
    }
}

template<typename T>
void SpectrumAnalyzerBands<T>::getAmplitudes(T *amplitudes) {
    for(int i=0; i<spectrumAnalyzerBands.size(); i++) {
        amplitudes[i] = spectrumAnalyzerBands[i].getMagnitude();
    }
}

template<typename T>
void SpectrumAnalyzerBands<T>::getAmplitudes(T *amplitudes, size_t beginningIndex) {
    for(int i=beginningIndex; i<spectrumAnalyzerBands.size(); i++) {
        amplitudes[i-beginningIndex] = spectrumAnalyzerBands[i].getMagnitude();
    }
}

template<typename T>
void
SpectrumAnalyzerBands<T>::getAmplitudes(T *amplitudes, size_t beginningIndex, size_t endingIndex) {
    for(int i=beginningIndex; i<endingIndex; i++) {
        amplitudes[i-beginningIndex] = spectrumAnalyzerBands[i].getMagnitude();
    }
}
