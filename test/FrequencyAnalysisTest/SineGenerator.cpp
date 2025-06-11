#include "SineGenerator.hpp"

#include <cmath>
#include <cassert>
#include <cstddef>
#include <numbers>

SineGenerator::SineGenerator(int tableSize) {
    this->tableSize = tableSize;
    this->phase = 0;
    double twoPi = std::numbers::pi * 2.0;
    table.resize(tableSize);
    for (int i = 0; i < tableSize; ++i) {
        table[i] = 0.125f * sin(((double)i/(double)tableSize)*twoPi);
    }
}

SineGenerator::~SineGenerator() {
}

int SineGenerator::read(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags) {
    assert(outputBuffer != nullptr);

    float **out = static_cast<float **>(outputBuffer);

    for (unsigned int i = 0; i < framesPerBuffer; ++i) {
        out[0][i] = table[phase];
        out[1][i] = table[phase];

        phase++;
        if (phase >= tableSize)
            phase -= tableSize;
    }

    return paContinue;
}
