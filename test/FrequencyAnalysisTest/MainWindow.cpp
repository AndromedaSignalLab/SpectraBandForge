/*
FrequencyBandTest main window definitions of SpectraBandForge Project
Copyright (C) 2025 Volkan Orhan

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "MainWindow.hpp"
#include "./ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->listWidget->setCurrentRow(0);
}

MainWindow::~MainWindow()
{
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
    int bandDesignator = 0;
    int bandAmount = 0;
    switch(index) {
    case 0:
        bandDesignator = 1;
        break;
    case 1:
        bandDesignator = 2;
        break;
    case 2:
        bandDesignator = 3;
        break;
    case 3:
        bandDesignator = 4;
        break;
    case 4:
        bandDesignator = 6;
        break;
    case 5:
        bandDesignator = 8;
        break;
    case 6:
        bandDesignator = 12;
        break;
    case 7:
        bandDesignator = 24;
        break;
    default:
        bandDesignator = 1;
        break;
    }
    bandAmount = bandDesignator * 10;
    ui->bandDesignatorLabel->setText(QString::number(bandDesignator));
    ui->bandAmountLabel->setText(QString::number(bandAmount));
}
