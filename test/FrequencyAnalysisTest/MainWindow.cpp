/*
FrequencyBandTest main window definitions of SpectraBandForge Project
Copyright (C) 2025 Volkan Orhan

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "MainWindow.hpp"
#include "BandFilter.hpp"

#include <iostream>

#include "./ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->listWidget->setCurrentRow(0);
    portaudio::System::initialize();

    initAudioIcons();
    initBitDepthValues();
    initSamplingFrequencyValues();
    initAudioInterfaceList();
    initAnalysisModeComboBox();
    updateAnalysisMode();
}

MainWindow::~MainWindow() {
    delete ui;
}

double threeWayRound(double value) {
    double int_part = std::floor(value);
    double frac = value - int_part;

    if (int_part < 1.0) {
        return std::round(frac * 100.0) / 100.0;
    }

    if (frac < 0.25) {
        return int_part;
    } else if (frac > 0.75) {
        return int_part + 1.0;
    } else {
        return int_part + 0.5;
    }
}

double xToFrequency(double x, double f_min, double f_max, double width) {
    if (x < 0) x = 0;
    if (x > width) x = width;

    double log_min = std::log10(f_min);
    double log_max = std::log10(f_max);

    double normalized = x / width;
    double log_f = normalized * (log_max - log_min) + log_min;
    return std::pow(10.0, log_f);
}

double beautifulFrequency(double frequency, bool &iskHz) {
    double output;
    if(frequency < 1000) {
        iskHz = false;
        output = threeWayRound(frequency);
    }
    else {
        iskHz = true;
        output = frequency / 1000;
        output = threeWayRound(output);
    }
    return output;
}

void MainWindow::on_listWidget_currentRowChanged(int currentRow) {
    ui->stackedWidget->setCurrentIndex(currentRow);
}

void MainWindow::on_frequencySlider_valueChanged(int value) {
    double f_min = 0.5;
    double f_max = 22000.0;
    double width = ui->frequencySlider->maximum()- ui->frequencySlider->minimum();

    double frequency = xToFrequency(value, f_min, f_max, width);
    bool isKhz;
    frequency = beautifulFrequency(frequency, isKhz);
    if(isKhz)
        ui->frequencyUnitLabel->setText("kHz");
    else
        ui->frequencyUnitLabel->setText("Hz");
    //double x = (log10(value) - log10(f_min)) / (log10(f_max) - log10(f_min)) * w;
    ui->frequencyLabel->setText(QString::number(frequency));
}

void MainWindow::on_analysisModeComboBox_currentIndexChanged(int index) {
    updateAnalysisMode();
}

void MainWindow::initAnalysisModeComboBox() {
    //Let item data hold band designator value

    for (int i = 0; i < bandDesignators.size(); i++) {
        ui->analysisModeComboBox->setItemData(i, bandDesignators[i]);
    }
}

void MainWindow::updateAnalysisMode() {
    ui->octaveBandsTable->model()->removeRows(0, ui->octaveBandsTable->rowCount());
    bandDesignator = ui->analysisModeComboBox->currentData().toInt();
    bandAmount = bandDesignator * 10;
    ui->bandDesignatorLabel->setText(QString::number(bandDesignator));
    ui->bandAmountLabel->setText(QString::number(bandAmount));
    ui->octaveBandsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    std::vector<OctaveBand<double>> octaveBands = BandFilter<double>::calculateOctaveBands(bandDesignator);
    int currentRowCount = 0;
    for (OctaveBand<double> &octaveBand : octaveBands) {
        ui->octaveBandsTable->insertRow(currentRowCount++);
        ui->octaveBandsTable->setItem(currentRowCount-1, 0, new QTableWidgetItem(QString::number(octaveBand.indexX)));
        ui->octaveBandsTable->setItem(currentRowCount-1, 1, new QTableWidgetItem(QString::number(octaveBand.nominalMidBandFrequency)));
        ui->octaveBandsTable->setItem(currentRowCount-1, 2, new QTableWidgetItem(QString::number(octaveBand.exactMidBandFrequency)));
        ui->octaveBandsTable->setItem(currentRowCount-1, 3, new QTableWidgetItem(QString::number(octaveBand.lowerEdgeBandFrequency)));
        ui->octaveBandsTable->setItem(currentRowCount-1, 4, new QTableWidgetItem(QString::number(octaveBand.upperEdgeBandFrequency)));
        ui->octaveBandsTable->setItem(currentRowCount-1, 5, new QTableWidgetItem(QString::number(octaveBand.upperEdgeBandFrequency - octaveBand.lowerEdgeBandFrequency)));
    }
}

void MainWindow::initAudioIcons() {
    iconCoreAudio.addPixmap(QPixmap(":/Resources/Graphics/Raster/CoreAudio.png"));
    iconWdmAudio.addPixmap(QPixmap(":/Resources/Graphics/Raster/WDM.png"));
    iconDirectXAudio.addPixmap(QPixmap(":/Resources/Graphics/Raster/DirectX.png"));
    iconAlsaAudio.addPixmap(QPixmap(":/Resources/Graphics/Raster/alsa.png"));
    iconOssAudio.addPixmap(QPixmap(":/Resources/Graphics/Raster/oss.png"));
    iconAsioAudio.addPixmap(QPixmap(":/Resources/Graphics/Raster/ASIO.png"));
    iconJackAudio.addPixmap(QPixmap(":/Resources/Graphics/Raster/JackAudio.png"));
}

void MainWindow::addDeviceToDeviceList(portaudio::Device & device) {
  std::cout << "--------------------------------------- device #" << device.index() << std::endl;

  // Mark global and API specific default devices:
  bool defaultDisplayed = false;
  QIcon * icon = &emptyIcon;

  if(device.isSystemDefaultInputDevice()) {
    std::cout << "[ Default Input";
    defaultDisplayed = true;
  }
  else if(device.isHostApiDefaultInputDevice()) {
    std::cout << "[ Default " << device.hostApi().name() << " Input";
    defaultDisplayed = true;
  }

  if(device.isSystemDefaultOutputDevice()) {
    std::cout << (defaultDisplayed ? "," : "[");
    std::cout << " Default Output";
    defaultDisplayed = true;
  }
  else if (device.isHostApiDefaultOutputDevice()) {
    std::cout << (defaultDisplayed ? "," : "[");
    std::cout << " Default " << device.hostApi().name() << " Output";
    defaultDisplayed = true;
  }

  if(defaultDisplayed)
    std::cout << " ]" << std::endl;
  if(!device.isInputOnlyDevice()) {
    QString devStr = QString("%1 - %2").arg(device.hostApi().name(), device.name());
    if(device.isSystemDefaultOutputDevice() || device.isHostApiDefaultOutputDevice())
      devStr += " (Default)";
    switch(device.hostApi().typeId()) {
    case PaHostApiTypeId::paALSA:
      icon = &iconAlsaAudio;
      break;
    case PaHostApiTypeId::paOSS:
      icon = &iconOssAudio;
      break;
    case PaHostApiTypeId::paCoreAudio:
      icon = &iconCoreAudio;
      break;
    case PaHostApiTypeId::paDirectSound:
      icon = &iconDirectXAudio;
      break;
    case PaHostApiTypeId::paWDMKS:
    case PaHostApiTypeId::paWASAPI:
    case PaHostApiTypeId::paMME:
      icon = &iconWdmAudio;
      break;
    case PaHostApiTypeId::paJACK:
      icon = &iconAsioAudio;
      break;
    default:
      break;
    }

    ui->comboBoxSoundDevices->addItem(*icon, devStr, device.index());
    qDebug() << "Device Name: " << devStr;
    qDebug() << "Device Id: " << device.index();
  }
  // Print device info:
  std::cout << "Name                        = " << device.name() << std::endl;
  std::cout << "Host API                    = " << device.hostApi().name() << std::endl;
  std::cout << "Max inputs = " << device.maxInputChannels() << ", Max outputs = " << device.maxOutputChannels() << std::endl;

  std::cout << "Default low input latency   = " << device.defaultLowInputLatency() << std::endl; // 8.3
  std::cout << "Default low output latency  = " << device.defaultLowOutputLatency() << std::endl; // 8.3
  std::cout << "Default high input latency  = " << device.defaultHighInputLatency() << std::endl; // 8.3
  std::cout << "Default high output latency = " << device.defaultHighOutputLatency() << std::endl; // 8.3
}

void MainWindow::initAudioInterfaceList() {
  portaudio::System &sys = portaudio::System::instance();

  try {
    addDeviceToDeviceList(sys.defaultOutputDevice());

    for (portaudio::System::DeviceIterator device = sys.devicesBegin();
         device != sys.devicesEnd(); ++device) {
      if (device->isSystemDefaultOutputDevice() || device->isInputOnlyDevice())
        continue;
      addDeviceToDeviceList(*device);
    }
  } catch (const portaudio::PaException &e) {
    std::cout << "A PortAudio error occured: " << e.paErrorText() << std::endl;
  } catch (const portaudio::PaCppException &e) {
    std::cout << "A PortAudioCpp error occured: " << e.what() << std::endl;
  } catch (const std::exception &e) {
    std::cout << "A generic exception occured: " << e.what() << std::endl;
  } catch (...) {
    std::cout << "An unknown exception occured." << std::endl;
  }
}
void MainWindow::initBitDepthValues() {
    ui->bitDepths->setItemData(0, QVariant(8));
    ui->bitDepths->setItemData(1, QVariant(16));
    ui->bitDepths->setItemData(2, QVariant(24));
}
void MainWindow::initSamplingFrequencyValues() {
    ui->samplingFrequencies->setItemData(0, QVariant(11000));
    ui->samplingFrequencies->setItemData(1, QVariant(22000));
    ui->samplingFrequencies->setItemData(2, QVariant(24000));
    ui->samplingFrequencies->setItemData(3, QVariant(44100));
    ui->samplingFrequencies->setItemData(4, QVariant(48000));
    ui->samplingFrequencies->setItemData(5, QVariant(96000));
    ui->samplingFrequencies->setItemData(6, QVariant(192000));

}