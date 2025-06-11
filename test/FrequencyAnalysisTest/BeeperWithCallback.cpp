#include "BeeperWithCallback.hpp"
#include "SineGenerator.hpp"
#include <QDebug>
#include <cassert>
#include <portaudiocpp/PortAudioCpp.hxx>

constexpr int NUM_SECONDS = 4;
constexpr double SAMPLE_RATE = 44100.0;
constexpr int FRAMES_PER_BUFFER = 64;


int BeeperWithCallback::open()
{
    SineGenerator sineGenerator;


    // Set up the parameters required to open a (Callback)Stream:
    portaudio::System & portAudioSys = portaudio::System::instance();
    portaudio::DirectionSpecificStreamParameters outParams(portAudioSys.defaultOutputDevice(), 2, portaudio::FLOAT32, false, portAudioSys.defaultOutputDevice().defaultLowOutputLatency(), nullptr);
    portaudio::StreamParameters params(portaudio::DirectionSpecificStreamParameters::null(), outParams, SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff);

    //portaudio::MemFunCallbackStream<SineGenerator> stream(params, sineGenerator, &SineGenerator::generate);
    portaudio::MemFunCallbackStream<SineGenerator> stream;
    stream.open(params, sineGenerator, &SineGenerator::read);

    qDebug()<<"Stream started";
    stream.start();

    portAudioSys.sleep(NUM_SECONDS * 1000);
    sineGenerator.setFrequency(880);
    sineGenerator.setVolume(0.5);
    portAudioSys.sleep(NUM_SECONDS * 1000);


    stream.stop();
    qDebug()<<"Stream stopped";

    stream.close();

    //for(auto device=portAudioSys.devicesBegin(); device != portAudioSys.devicesEnd(); ++device)
    //    qDebug()<<device->name();
    qDebug()<<portAudioSys.deviceCount();
    qDebug()<<portAudioSys.defaultOutputDevice().index();

    //portAudioSys.sleep(NUM_SECONDS * 1000);
    return 0;

}
int BeeperWithCallback::play() {
    return 0;
}
