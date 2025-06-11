#include "SineGenerator.hpp"

#include <cmath>
#include <cassert>
#include <cstddef>
#include <numbers>

SineGenerator::SineGenerator(int tableSize) {
    this->tableSize = tableSize;
    this->leftPhase = 0;
    this->rightPhase = 0;
    double twoPi = std::numbers::pi * 2.0;
    table = new float[tableSize];
    for (int i = 0; i < tableSize; ++i) {
        table[i] = 0.125f * sin(((double)i/(double)tableSize)*twoPi);
    }
}

SineGenerator::~SineGenerator() {
    delete[] table;
}

int SineGenerator::read(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags) {
    assert(outputBuffer != nullptr);

    float **out = static_cast<float **>(outputBuffer);

    for (unsigned int i = 0; i < framesPerBuffer; ++i) {
        out[0][i] = table[leftPhase];
        out[1][i] = table[rightPhase];

        leftPhase += 1;
        if (leftPhase >= tableSize)
            leftPhase -= tableSize;

        rightPhase += 3;
        if (rightPhase >= tableSize)
            rightPhase -= tableSize;
    }

    return paContinue;
}
