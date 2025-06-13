/*
FrequencyBandTest SineGenerator declarations of SpectraBandForge Project
Copyright (C) 2025 Volkan Orhan

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/
#pragma once
#include <portaudiocpp/PortAudioCpp.hxx>
#include  <vector>
#include  <mutex>

// SineGenerator class:
class SineGenerator {
public:
    SineGenerator(const int sineTableSize = 8192);

    int read(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags);

    int generate(float *outputBuffer, const unsigned long size, const bool addToPreviousWave = false);
    int generateStereo(void *outputBuffer, const unsigned long size, const bool addToPreviousWave = false);

    void setFrequency(const double frequency);
    double getFrequency() const;
    void setSampleRate(const double sampleRate);
    double getSampleRate() const;
    void setVolume(const double volume);
    double getVolume() const;
    void setGain(const double gain);
    double getGain() const;

private:
    std::mutex soundDataMutex;
    std::vector<float> sineTable;
    int sineTableSize;
    int phase;
    double frequency = 440.0;
    double sampleRate = 44100.0;
    double gain;
    double volume;
};
