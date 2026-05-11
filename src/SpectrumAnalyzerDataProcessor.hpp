/*
SpectrumAnalyzerDataProcessor class declarations of Andromeda Signal Lab
Copyright (C) 2025 Volkan Orhan

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include <QObject>
#include <BandFilter.hpp>
#include "Interfaces/FFT.hpp"
#include <AndromedaStructures.hpp>

using namespace AndromedaSignalLab;

template <class SampleDataType, class SpectrumDataType, class FFTDataType = SampleDataType>
class SpectrumAnalyzerDataProcessor {
public:
    explicit SpectrumAnalyzerDataProcessor(const int bandDesignator, std::timed_mutex &soundDataMutex, const size_t framesPerBuffer, const size_t fftSize, const SoundResolution soundResolution, const WindowFunction windowFunction);
    ~SpectrumAnalyzerDataProcessor();
    int getBarAmount();
    void calculateSpectrumData(size_t inputDataCount, SampleDataType *leftSoundChannelData, SampleDataType *rightSoundChannelData, SpectrumDataType *spectrumData);
private:
    size_t framesPerBuffer = 0;
    size_t fftSize = 0;
    double frequencySpacing = 0;
    size_t spectrumAnalyzerBarAmount = 0;
    const int bandDesignator = 0;
    SpectrumAnalyzerBands<double> spectrumAnalyzerBands;
    AndromedaSignalLab::Interfaces::FFT<FFTDataType> *fft = nullptr;
    SoundResolution soundResolution;
    std::timed_mutex &soundDataMutex;
    SampleDataType *windowMultipliers = nullptr;
    WindowFunction windowFunction = WindowFunction::None;
    void initalize(const size_t framesPerBuffer, const size_t fftSize, const SoundResolution soundResolution, const WindowFunction windowFunction);
    void updateFFT(size_t inputDataCount, SampleDataType *leftSoundChannelData, SampleDataType *rightSoundChannelData, SpectrumDataType *spectrumData);
    void setWindowFunction(const WindowFunction windowFunction);

};

#include "SpectrumAnalyzerDataProcessorDefinitions.hpp"