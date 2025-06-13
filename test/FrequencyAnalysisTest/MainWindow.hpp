/*
FrequencyBandTest main window declarations of SpectraBandForge Project
Copyright (C) 2025 Volkan Orhan

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#pragma once

#include "SineGenerator.hpp"

#include <QMainWindow>
#include <portaudiocpp/PortAudioCpp.hxx>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_listWidget_currentRowChanged(int currentRow);

    void on_frequencySlider_valueChanged(int value);

    void on_sineSweepAmplitudeSlider_valueChanged(int value);

    void on_analysisModeComboBox_currentIndexChanged(int index);

    void on_startOrStopSineSweepAnalysisButton_clicked();

  private:
    Ui::MainWindow *ui;
    void initAnalysisModeComboBox();
    void updateAnalysisMode();
    void addDeviceToDeviceList(portaudio::Device & device);
    void initAudioIcons();
    void initAudioInterfaceList();
    void initBitDepthValues();
    void initSamplingFrequencyValues();
    void initAudio();
    const QList<int> bandDesignators = {1, 2, 3, 4, 6, 8, 12, 24};
    int bandDesignator = 0;
    int bandAmount = 0;
    QIcon iconCoreAudio;
    QIcon iconWdmAudio;
    QIcon iconDirectXAudio;
    QIcon iconAlsaAudio;
    QIcon iconOssAudio;
    QIcon iconAsioAudio;
    QIcon iconJackAudio;
    QIcon emptyIcon;
    SineGenerator sineGenerator;
    portaudio::MemFunCallbackStream<SineGenerator> stream;
    bool sineSweepAnalysisStarted = false;
};
