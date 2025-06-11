#pragma once
#include <portaudiocpp/PortAudioCpp.hxx>
#include  <vector>


// Some constants:
const int NUM_SECONDS = 4;
const double SAMPLE_RATE = 44100.0;
const int FRAMES_PER_BUFFER = 64;
const int TABLE_SIZE = 200;

// SineGenerator class:
class SineGenerator {
public:
    SineGenerator(int tableSize);
    ~SineGenerator();

    int read(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags);

private:
    std::vector<float> table;
    int tableSize;
    int phase;
};
