/*
SpectrumAnalyzerDataProcessor class definitions of ModPlug Player
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

class SpectrumAnalyzerDataProcessor : public QObject
{
    Q_OBJECT
public:
    explicit SpectrumAnalyzerDataProcessor(const int bandDesignator, std::timed_mutex &soundDataMutex, const size_t bufferSize, const size_t framesPerBuffer, const SoundResolution soundResolution, const WindowFunction windowFunction);
    ~SpectrumAnalyzerDataProcessor();
    void calculateSpectrumData(size_t readCount, float *leftSoundChannelData, float *rightSoundChannelData, double *spectrumData);
private:
    double frequencySpacing = 0;
    int fftPrecision = 0;
    size_t spectrumAnalyzerBarAmount = 20;
    const int bandDesignator = 0;
    SpectrumAnalyzerBands<double> spectrumAnalyzerBands;
    Interfaces::FFT<float> *fft = nullptr;
    SoundResolution soundResolution;
    std::timed_mutex &soundDataMutex;
    float *windowMultipliers = nullptr;
    WindowFunction windowFunction = WindowFunction::None;
    size_t bufferSize = 0;
    size_t framesPerBuffer = 0;
    void initalize(const size_t bufferSize, const size_t framesPerBuffer, const SoundResolution soundResolution, const WindowFunction windowFunction);
    void updateFFT(size_t inputDataCount, float *leftSoundChannelData, float *rightSoundChannelData, double *spectrumData);
    void setWindowFunction(const WindowFunction windowFunction);
};
