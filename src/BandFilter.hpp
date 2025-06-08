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
    static inline std::vector<OctaveBand<T>> calculateOctaveBands(size_t b);
    static inline std::vector<OctaveBand<T>> calculateOctaveBandsByFrequencyInterval(OctaveBandBase base, size_t nthOctave, T minFreq = 20, T maxFreq = 20000);
    static inline std::vector<OctaveBand<T>> calculateOctaveBandsByXIndices(OctaveBandBase base, size_t b, int xBeginning, int xEnding);
    /*
    template<class FFTDataType> static inline
    std::vector<SpectrumAnalyzerBandDTO<T>>
    calculateSpectrumAnalyzerBands(const std::vector<OctaveBand<T>> &octaveBands, FFTDataType *fftData, size_t frameAmount, size_t sampleRate);
    */
private:
    static inline void getIndexBoundaries(const size_t nthOctave, int &lowerIndex, int &upperIndex, bool &isValid);
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

template<class T> inline void BandFilter<T>::getIndexBoundaries(const size_t b, int &lowerIndex, int &upperIndex, bool &isValid) {
    if (!(b == 1 || b == 2 || b == 3 || b == 4 || b == 6 || b == 8 || b == 12 || b == 24)) {
        isValid = false;
        return;
    }
    isValid = true;
    switch (b) {
        case 1:
        lowerIndex = -5;
        upperIndex = 5;
        break;
    case 2:
        lowerIndex = -11;
        upperIndex = 9;
        break;
    case 3:
        lowerIndex = -16;
        upperIndex = 14;
        break;
    case 4:
        lowerIndex = -23;
        upperIndex = 17;
        break;
    case 6:
        lowerIndex = -34;
        upperIndex = 26;
        break;
    case 8:
        lowerIndex = -45;
        upperIndex = 35;
        break;
    case 12:
        lowerIndex = -68;
        upperIndex = 52;
        break;
    case 24:
        lowerIndex = -136;
        upperIndex = 104;
        break;
    }
}

// Inline Method Definitions
/*
 * "ANSI S1.11: Specification for Octave, Half-Octave, and Third Octave Band Filter Sets" (PDF). Retrieved 7 March 2018.
 *
 * \param b Bandwidth designator.
 * Corresponds to "n" in the 1/n octave band structure.
 * For example, b is 3 for 1/3 octave bands.
*/
template<class T> inline std::vector<OctaveBand<T>> BandFilter<T>::calculateOctaveBandsByXIndices(OctaveBandBase base, size_t b, int xBeginning, int xEnding) {
    const T fr = T(1000); //reference frequency
    const T G2 = T(2);
    const double G10 = pow(T(10), T(3)/T(10));

    T fm; //exact midband frequency
    T fm10;
    std::vector<OctaveBand<T>> octaveBands;
    T G = (base == OctaveBandBase::Base10) ? G10 : G2;

    for(int x = xBeginning; x < xEnding; x++) {
        OctaveBand<T> octaveBand;
        fm = FrequencyCalculator::calculateExactMidBandFrequency(b, G, fr, x);
        fm10 = (base == OctaveBandBase::Base10) ? fm : FrequencyCalculator::calculateExactMidBandFrequency(b, G10, fr, x);
        octaveBand.indexX = x;
        octaveBand.base = base;
        octaveBand.midBandFrequency = fm;
        octaveBand.lowerEdgeBandFrequency = FrequencyCalculator::calculateLowerEdgeBandFrequency(b, G, fm);
        octaveBand.upperEdgeBandFrequency = FrequencyCalculator::calculateUpperEdgeBandFrequency(b, G, fm);
        octaveBand.nominalMidBandFrequency = FrequencyCalculator::calculateNominalFrequency<T>(b, fm10);
        octaveBands.push_back(octaveBand);
    }

    return octaveBands;
}


// Inline Method Definitions
/*
 * "ANSI S1.11: Specification for Octave, Half-Octave, and Third Octave Band Filter Sets" (PDF). Retrieved 7 March 2018.
 */
template<class T> inline std::vector<OctaveBand<T>> BandFilter<T>::calculateOctaveBandsByFrequencyInterval(OctaveBandBase base, size_t b, T minFreq, T maxFreq) {
    const T fr = T(1000); //reference frequency
    const T G2 = T(2);
    const double G10 = pow(T(10), T(3)/T(10));

    T fm; //exact midband frequency
    T fm10;
    std::vector<OctaveBand<T>> octaveBands;
    int x = -200;
    //int currentBandIndex = 0;
    T G = (base == OctaveBandBase::Base10) ? G10 : G2;

    while(true) {
        OctaveBand<T> octaveBand;
        fm = FrequencyCalculator::calculateExactMidBandFrequency(b, G, fr, x);
        fm10 = (base == OctaveBandBase::Base10) ? fm : FrequencyCalculator::calculateExactMidBandFrequency(b, G10, fr, x);
        octaveBand.indexX = x;
        octaveBand.base = base;
        octaveBand.midBandFrequency = fm;
        octaveBand.lowerEdgeBandFrequency = FrequencyCalculator::calculateLowerEdgeBandFrequency(b, G, fm);
        octaveBand.upperEdgeBandFrequency = FrequencyCalculator::calculateUpperEdgeBandFrequency(b, G, fm);
        if(octaveBand.upperEdgeBandFrequency < minFreq) {
            x++;
            continue;
        }
        octaveBand.nominalMidBandFrequency = FrequencyCalculator::calculateNominalFrequency<T>(b, fm10);
        if(octaveBand.lowerEdgeBandFrequency > maxFreq)
            break;
        octaveBands.push_back(octaveBand);
        x++;
        //currentBandIndex++;
    }

    return octaveBands;
}

template<class T>  inline std::vector<OctaveBand<T>> BandFilter<T>::calculateOctaveBands(size_t b) {
    int lowerIndex = 0;
    int upperIndex = 0;
    bool isValid = false;
    getIndexBoundaries(b, lowerIndex, upperIndex, isValid);
    if (isValid) {
        return calculateOctaveBandsByXIndices(OctaveBandBase::Base10, b, lowerIndex, upperIndex);
    }
    else {
        return calculateOctaveBandsByFrequencyInterval(OctaveBandBase::Base10, b, 20, 20000);
    }
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
        void setBandWidthDesignator(size_t bandwidthDesignator);
        void setLowerIndexX(int indexX);
        void setHigherIndexX(int indexX);
        SpectrumAnalyzerBandDTO<T> & operator [](const int indexX);
        void resetMagnitudes();
        std::vector<SpectrumAnalyzerBandDTO<T>> getData();
        void getData(SpectrumAnalyzerBandDTO<T> * bandData);
        void getAmplitudes(T * amplitudes);
        void getAmplitudes(T * amplitudes, size_t beginningIndex);
        void getAmplitudes(T * amplitudes, size_t beginningIndex, size_t endingIndex);
    private:
        void init();
        std::vector<SpectrumAnalyzerBandDTO<T>> spectrumAnalyzerBands;
};

template<typename T>
SpectrumAnalyzerBandDTO<T> &SpectrumAnalyzerBands<T>::operator[](const int indexX) {
    for(SpectrumAnalyzerBandDTO<T> &spectrumAnalyzerBand:spectrumAnalyzerBands){
        if(spectrumAnalyzerBand.bandInfo.indexX == indexX)
            return spectrumAnalyzerBand;
    }
    throw std::exception();
}

template<typename T>
void SpectrumAnalyzerBands<T>::init() {
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
