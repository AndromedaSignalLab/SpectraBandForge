/*
FrequencyBandTest SineGenerator declarations of SpectraBandForge Project
Copyright (C) 2025 Volkan Orhan

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/
#pragma once
#include <mutex>
#include <numbers>
#include <portaudiocpp/PortAudioCpp.hxx>
#include <vector>

// SineGenerator class:
class SineGenerator {
  public:
    SineGenerator(const int sineTableSize = 32768);

    int generate(float* outputBuffer, const unsigned long size, const bool addToPreviousWave = false);
    int generateStereo(float** outputBuffer, const unsigned long size, const bool addToPreviousWave = false);

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
    double phase;
    double phaseIncrement;
    double frequency = 440.0;
    double sampleRate = 44100.0;
    double gain;
    double volume;
    static constexpr double twoPi = std::numbers::pi * 2.0;
    bool useTable = true;
};
