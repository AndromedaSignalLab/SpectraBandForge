#include "BeeperWithCallback.hpp"
#include <QDebug>
#include <cmath>
#include <cassert>
#include <cstddef>
#include <portaudiocpp/PortAudioCpp.hxx>
#include "Enums.hpp"
#include "SineGenerator.hpp"

int BeeperWithCallback::open()
{
    SineGenerator sineGenerator(TABLE_SIZE);


    // Set up the parameters required to open a (Callback)Stream:
    portaudio::System & portAudioSys = portaudio::System::instance();
    portaudio::DirectionSpecificStreamParameters outParams(portAudioSys.defaultOutputDevice(), 2, portaudio::FLOAT32, false, portAudioSys.defaultOutputDevice().defaultLowOutputLatency(), NULL);
    portaudio::StreamParameters params(portaudio::DirectionSpecificStreamParameters::null(), outParams, SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff);

    //portaudio::MemFunCallbackStream<SineGenerator> stream(params, sineGenerator, &SineGenerator::generate);
    portaudio::MemFunCallbackStream<SineGenerator> stream;
    stream.open(params, sineGenerator, &SineGenerator::generate);

    qDebug()<<"Stream started";
    stream.start();

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
