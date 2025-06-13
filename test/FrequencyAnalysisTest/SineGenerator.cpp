/*
FrequencyBandTest SineGenerator definitions of SpectraBandForge Project
Copyright (C) 2025 Volkan Orhan

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/
#include "SineGenerator.hpp"

#include <cmath>
#include <cassert>
#include <numbers>

SineGenerator::SineGenerator(const int sineTableSize) {
    this->sineTableSize = sineTableSize;
    this->phase = 0;
    double twoPi = std::numbers::pi * 2.0;
    sineTable.resize(sineTableSize);
    setGain(0.125f);
    for (int i = 0; i < sineTableSize; ++i) {
        sineTable[i] = sin(((double)i/(double)sineTableSize)*twoPi);
        //table[i] = static_cast<float>(std::sin((2.0 * M_PI * i) / tableSize));
    }
}

void SineGenerator::setFrequency(const double freq) {
    soundDataMutex.lock();
    frequency = freq;
    soundDataMutex.unlock();
}

double SineGenerator::getFrequency() const {
    return frequency;
}

void SineGenerator::setSampleRate(const double sampleRate) {
    soundDataMutex.lock();
    this->sampleRate = sampleRate;
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
    volume = 20*log10(gain);
    soundDataMutex.unlock();
}

double SineGenerator::getGain() const {
    return gain;
}

int SineGenerator::read(const void *inputBuffer, void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo *timeInfo,
                        PaStreamCallbackFlags statusFlags) {
  return generateStereo(outputBuffer, framesPerBuffer, false);
}

int SineGenerator::generate(float *outputBuffer, unsigned long size,
                            bool addToPreviousWave) {
    assert(outputBuffer != nullptr);
    soundDataMutex.lock();
    for (unsigned int i = 0; i < size; ++i) {
      if (addToPreviousWave)
        outputBuffer[i] += sineTable[phase] * gain;
      else
        outputBuffer[i] = sineTable[phase] * gain;
      phase += static_cast<int>((frequency * sineTableSize) / sampleRate);
      if (phase >= sineTableSize)
        phase -= sineTableSize;
    }
    soundDataMutex.unlock();
    return paContinue;
}

int SineGenerator::generateStereo(void *outputBuffer, unsigned long size,
                                  bool addToPreviousWave) {
  assert(outputBuffer != nullptr);

  float **out = static_cast<float **>(outputBuffer);

  soundDataMutex.lock();
  for (unsigned int i = 0; i < size; ++i) {
    out[0][i] = sineTable[phase] * gain;
    out[1][i] = sineTable[phase] * gain;

    phase += static_cast<int>((frequency * sineTableSize) / sampleRate);
    if (phase >= sineTableSize)
      phase -= sineTableSize;
  }
  soundDataMutex.unlock();
  return paContinue;
}
