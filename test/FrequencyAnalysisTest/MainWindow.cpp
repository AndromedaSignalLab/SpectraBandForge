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
    initAnalysisModeComboBox();
    updateAnalysisMode();
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
