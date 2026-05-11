/*
SpectrumAnalyzerDataProcessor class definitions of Andromeda Signal Lab
Copyright (C) 2025 Volkan Orhan

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/
//#include "Implementation/FFT/KissFFTImpl.hpp"
#include "Implementations/FFT/FFTWImpl.hpp"
#include <AndromedaDSP.hpp>
#include <QDebug>

#include "FFTUtil.hpp"

template<class SampleDataType, class SpectrumDataType, class FFTDataType>
SpectrumAnalyzerDataProcessor<SampleDataType, SpectrumDataType, FFTDataType>::SpectrumAnalyzerDataProcessor(const int bandDesignator, std::timed_mutex &soundDataMutex, const size_t bufferSize, const size_t framesPerBuffer, const SoundResolution soundResolution, const WindowFunction windowFunction)
: soundDataMutex(soundDataMutex), bandDesignator(bandDesignator){
    //fft = new KissFFTImpl<float>();
    fft = new FFTWImpl<FFTDataType>();
    initalize(bufferSize, framesPerBuffer, soundResolution, windowFunction);
}

template<class SampleDataType, class SpectrumDataType, class FFTDataType>
SpectrumAnalyzerDataProcessor<SampleDataType, SpectrumDataType, FFTDataType>::~SpectrumAnalyzerDataProcessor() {
    if(fft == nullptr)
        return;
    if(fft->isOpen())
        fft->close();
    delete fft;
    fft = nullptr;
}

template<class SampleDataType, class SpectrumDataType, class FFTDataType>
int SpectrumAnalyzerDataProcessor<SampleDataType, SpectrumDataType, FFTDataType>::getBarAmount() {
    return spectrumAnalyzerBarAmount;
}

template<class SampleDataType, class SpectrumDataType, class FFTDataType>
void SpectrumAnalyzerDataProcessor<SampleDataType, SpectrumDataType, FFTDataType>::calculateSpectrumData(
    size_t inputDataCount, SampleDataType *leftSoundChannelData, SampleDataType *rightSoundChannelData,
    SpectrumDataType *spectrumData) {
    //if(playerState == PlayingState::Playing) {
    updateFFT(inputDataCount, leftSoundChannelData, rightSoundChannelData, spectrumData);
    this->spectrumAnalyzerBands.getAmplitudes(spectrumData);
    //}
    //else
    //    std::fill(spectrumData, spectrumData+20, 0);
}

template<class SampleDataType, class SpectrumDataType, class FFTDataType>
void SpectrumAnalyzerDataProcessor<SampleDataType, SpectrumDataType, FFTDataType>::initalize(const size_t bufferSize, const size_t framesPerBuffer, const SoundResolution soundResolution, const WindowFunction windowFunction) {
    this->bufferSize = bufferSize;
    this->framesPerBuffer = framesPerBuffer;
    this->fftPrecision = framesPerBuffer;
    this->frequencySpacing = double(soundResolution.sampleRate)/fftPrecision;
    std::vector<OctaveBand<SpectrumDataType>> bands = BandFilter<SpectrumDataType>::calculateOctaveBands(bandDesignator);
    spectrumAnalyzerBands = SpectrumAnalyzerBands<SpectrumDataType>(bands);
    spectrumAnalyzerBarAmount = bands.size();
    qDebug()<<"Spectrum analyzer bar amount is"<<spectrumAnalyzerBarAmount;
    //spectrumData->assign(spectrumAnalyzerBarAmount, 0);

    setWindowFunction(windowFunction);

    fft->initialize(framesPerBuffer);
}

template<class SampleDataType, class SpectrumDataType, class FFTDataType>
void SpectrumAnalyzerDataProcessor<SampleDataType, SpectrumDataType, FFTDataType>::updateFFT(size_t inputDataCount, SampleDataType *leftSoundChannelData, SampleDataType *rightSoundChannelData, SpectrumDataType *spectrumData) {
    if(spectrumData == nullptr)
        return;
    double magnitude;
    //double magnitude_dB;
    spectrumAnalyzerBands.resetMagnitudes();
    soundDataMutex.lock();
    if(windowFunction == WindowFunction::None) {
        for (unsigned int i = 0; i < inputDataCount; i++) {
            if(fft->fftInput != nullptr)
                fft->fftInput[i] = (leftSoundChannelData[i]/2 + rightSoundChannelData[i]/2);
        }
    }
    else {
        for (unsigned int i = 0; i < inputDataCount; i++) {
            if(fft->fftInput != nullptr)
                fft->fftInput[i] = (leftSoundChannelData[i]/2 + rightSoundChannelData[i]/2) * windowMultipliers[i];
        }
    }
    soundDataMutex.unlock();
    double mean = 0.0;

    FFTUtil::removeMean(fft->fftInput, inputDataCount);

    fft->execute();

    for(int i=0; i<fftPrecision; i++) {
        magnitude = AndromedaDSP::AndromedaDSP<double>::calculateMagnitude(fft->fftOutput[i].real(), fft->fftOutput[i].imag());
        //qDebug()<<"magnitude: "<<magnitude;
        int indexX = spectrumAnalyzerBands.getIndexXByFrequency(frequencySpacing*i);
        //int indexX = spectrumAnalyzerBands.getIndexXByFrequencyBin(frequencySpacing*i, frequencySpacing);
        if (indexX == InvalidBandIndex)
            continue;
        SpectrumAnalyzerBandDTO<double> & spectrumAnalyzerBand = spectrumAnalyzerBands[indexX];
        //if(spectrumAnalyzerBand.bandInfo.nominalMidBandFrequency >= 0 && !std::isnan(magnitude)){
        spectrumAnalyzerBand.addMagnitude(magnitude);
        //}
        //else
        //    qDebug()<<"nan magnitude";
        //spectrumData[i] = AndromedaDSP<double>::calculateMagnitudeDb(fftOutput[i][REAL], fftOutput[i][IMAG]);
        //qDebug()<<"Max Magnitude: "<<maxMagnitude<<" FFT Output["<<i<<"] Real: "<<QString::number(fftOutput[i][REAL], 'g', 6) << "Imaginary: "<<fftOutput[i][IMAG]<<" Magnitude: "<<magnitude<<" DB: "<<magnitude_dB;
    }
    for (int i=0; i<spectrumAnalyzerBarAmount; i++) {
        spectrumData[i] = spectrumAnalyzerBands.getBandByBandIndex(i).getAverageMagnitude();
    }
}

template<class SampleDataType, class SpectrumDataType, class FFTDataType>
void SpectrumAnalyzerDataProcessor<SampleDataType, SpectrumDataType, FFTDataType>::setWindowFunction(const WindowFunction windowFunction) {
    soundDataMutex.lock();
    this->windowFunction = windowFunction;
    if(windowMultipliers != nullptr) {
        delete[] windowMultipliers;
    }
    switch(windowFunction) {
        case WindowFunction::None:
            windowMultipliers = nullptr;
            break;
        case WindowFunction::HanningWindow:
            windowMultipliers = AndromedaDSP::AndromedaDSP<float>::hanningMultipliers(this->framesPerBuffer);
            break;
        case WindowFunction::HammingWindow:
            windowMultipliers = AndromedaDSP::AndromedaDSP<float>::hammingMultipliers(this->framesPerBuffer);
            break;
        case WindowFunction::BlackmanWindow:
            windowMultipliers = AndromedaDSP::AndromedaDSP<float>::blackmanMultipliers(this->framesPerBuffer);
            break;
    }
    soundDataMutex.unlock();
    //emit MessageCenter::getInstance().events.spectrumAnalyzerEvents.windowFunctionChanged(windowFunction);
}
