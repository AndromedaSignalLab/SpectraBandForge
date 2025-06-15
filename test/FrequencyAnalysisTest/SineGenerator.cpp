/*
FrequencyBandTest SineGenerator definitions of SpectraBandForge Project
Copyright (C) 2025 Volkan Orhan

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/
#include "SineGenerator.hpp"

#include <cassert>
#include <cmath>
#include <numbers>

SineGenerator::SineGenerator(const int sineTableSize) {
    this->sineTableSize = sineTableSize;
    this->phase = 0;
    updateCurrentVariables();
    sineTable.resize(sineTableSize);
    setGain(0.125f);
    for(int i = 0; i < sineTableSize; ++i) {
        sineTable[i] = sin(((double)i / (double)sineTableSize) * twoPi);
        // table[i] = static_cast<float>(std::sin((2.0 * M_PI * i) / tableSize));
    }
}

void SineGenerator::updateCurrentVariables() {
    tableIndexIncrement = (frequency * sineTableSize) / sampleRate;
    phaseIncrement = twoPi * frequency / sampleRate;
}

void SineGenerator::setFrequency(const double freq) {
    soundDataMutex.lock();
    frequency = freq;
    updateCurrentVariables();
    soundDataMutex.unlock();
}

double SineGenerator::getFrequency() const {
    return frequency;
}

void SineGenerator::setSampleRate(const double sampleRate) {
    soundDataMutex.lock();
    this->sampleRate = sampleRate;
    updateCurrentVariables();
    soundDataMutex.unlock();
}

double SineGenerator::getSampleRate() const {
    return sampleRate;
}

void SineGenerator::setVolume(const double volume) {
    std::lock_guard<std::mutex> lock(soundDataMutex);
    this->volume = volume;
    gain = pow(10, volume / 20.0);
}

double SineGenerator::getVolume() const {
    return volume;
}

void SineGenerator::setGain(const double gain) {
    std::lock_guard<std::mutex> lock(soundDataMutex);
    this->gain = std::max(gain, 0.00001);
    volume = 20 * log10(gain);
}

double SineGenerator::getGain() const {
    return gain;
}

int SineGenerator::generate(float* outputBuffer, unsigned long size,
                            bool addToPreviousWave) {
    assert(outputBuffer != nullptr);
    soundDataMutex.lock();
    for(unsigned int i = 0; i < size; ++i) {
        if(addToPreviousWave)
            outputBuffer[i] += sineTable[phase] * gain;
        else
            outputBuffer[i] = sineTable[phase] * gain;
        phase += static_cast<int>((frequency * sineTableSize) / sampleRate);
        if(phase >= sineTableSize)
            phase -= sineTableSize;
    }
    soundDataMutex.unlock();
    return paContinue;
}

int SineGenerator::generateStereo(float** outputBuffer, unsigned long size,
                                  bool addToPreviousWave) {
    assert(outputBuffer != nullptr);
    float sampleAmplitude;
    std::lock_guard<std::mutex> lock(soundDataMutex);
    for (unsigned int i = 0; i < size; ++i) {
        sampleAmplitude = useTable ? sineTable[phase] : std::sin(phase);
        sampleAmplitude *= gain;
        outputBuffer[0][i] = sampleAmplitude;
        outputBuffer[1][i] = sampleAmplitude;
        phase += useTable ? tableIndexIncrement : phaseIncrement;
        if(useTable) {
            if(phase >= sineTableSize)
                phase -= sineTableSize;
        } else {
            if (phase >= twoPi)
                phase -= twoPi;
        }
    }

    return paContinue;
}
