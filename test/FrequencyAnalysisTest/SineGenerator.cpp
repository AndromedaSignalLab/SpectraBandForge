#include "SineGenerator.hpp"

#include <cmath>
#include <cassert>
#include <cstddef>
#include <numbers>

SineGenerator::SineGenerator(int sineTableSize) {
    this->sineTableSize = sineTableSize;
    this->phase = 0;
    double twoPi = std::numbers::pi * 2.0;
    sineTable.resize(sineTableSize);
    for (int i = 0; i < sineTableSize; ++i) {
        sineTable[i] = sin(((double)i/(double)sineTableSize)*twoPi);
        //table[i] = static_cast<float>(std::sin((2.0 * M_PI * i) / tableSize));
    }
}

void SineGenerator::setFrequency(double freq) {
    soundDataMutex.lock();
    frequency = freq;
    soundDataMutex.unlock();
}

double SineGenerator::getFrequency() const {
    return frequency;
}

void SineGenerator::setSampleRate(double sampleRate) {
    soundDataMutex.lock();
    this->sampleRate = sampleRate;
    soundDataMutex.unlock();
}

double SineGenerator::getSampleRate() const {
    return sampleRate;
}

void SineGenerator::setVolume(double volume) {
    soundDataMutex.lock();
    this->volume = volume;
    soundDataMutex.unlock();
}

double SineGenerator::getVolume() const {
    return volume;
}

int SineGenerator::read(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags) {
    assert(outputBuffer != nullptr);

    float **out = static_cast<float **>(outputBuffer);

    soundDataMutex.lock();
    for (unsigned int i = 0; i < framesPerBuffer; ++i) {
        out[0][i] = sineTable[phase]*volume;
        out[1][i] = sineTable[phase]*volume;

        phase += static_cast<int>((frequency * sineTableSize) / sampleRate);
        if (phase >= sineTableSize)
            phase -= sineTableSize;
    }
    soundDataMutex.unlock();
    return paContinue;
}
