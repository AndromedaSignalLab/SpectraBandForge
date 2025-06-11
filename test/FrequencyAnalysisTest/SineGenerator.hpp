#pragma once
#include <portaudiocpp/PortAudioCpp.hxx>
#include  <vector>
#include  <mutex>

// SineGenerator class:
class SineGenerator {
public:
    SineGenerator(int sineTableSize = 8192);

    int read(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags);

    void setFrequency(double frequency);
    double getFrequency() const;
    void setSampleRate(double sampleRate);
    double getSampleRate() const;
    void setVolume(double volume);
    double getVolume() const;

private:
    std::mutex soundDataMutex;
    std::vector<float> sineTable;
    int sineTableSize;
    int phase;
    double frequency = 440.0;
    double sampleRate = 44100.0;
    double volume = 0.125f;
};
