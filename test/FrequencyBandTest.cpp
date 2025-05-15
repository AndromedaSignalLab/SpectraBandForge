/*
FrequencyBandTest test function definitions of SpectraBandForge Project
Copyright (C) 2020 Volkan Orhan

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <iostream>
#include "../src/BandFilter.hpp"
#include "FrequencyCalculator.hpp"
#include "BandDefinitions.hpp"
#include <ranges>
#include "DSP.hpp"

//31 Band:
//20    31.5   50   80    125   200   315   500   800   1.25k   2k   3.15k   5k   8k    12.5k   20k
//25    40     63   100   160   250   400   630   1k    1.6k   2.5k   4k    6.3k  10k     16k

//⅓ 8va bands
//1, 1.25, 1.63, 2, 2.5, 3.15, 4, 5, 6.3, 8, 10, 12.5, 16.3, 20, 25, 31.5, 40, 50, 63, 80, 100, 125, 163, 200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1630, 2k, 2500, 3150, 4k, 5k, 6300, 8k, 10k, 12,5k 16k, 20k, etc; as necessary

//8va bands

//31.5, 63, 125, 250, 500, 1000, 2k, 4k, 8k, 16k, etc; as necessary

void printOctaveBands(size_t nthOctave){
    std::vector<OctaveBand<double>> octaveBands;
    octaveBands = BandFilter<double>::calculateOctaveBands(OctaveBandBase::Base10, nthOctave);
    std::cout<<octaveBands.size()<<" frequencies"<<std::endl;

    for(OctaveBand<double> &o:octaveBands) {
        std::cout <<"Band Number: "<<o.bandNumber<<" Nominal Freq: "<< o.nominalMidBandFrequency <<" Exact Freq: "<<o.midBandFrequency<< " Upper Freq: " << o.upperEdgeBandFrequency << " Lower Freq:" << o.lowerEdgeBandFrequency << std::endl;
    }

}
void printSpectrumAnalyzerBands(size_t nthOctave){
    std::vector<OctaveBand<double>> octaveBands;
    octaveBands = BandFilter<double>::calculateOctaveBands(OctaveBandBase::Base10, nthOctave);
    std::cout<<octaveBands.size()<<" frequencies"<<std::endl;

    SpectrumAnalyzerBands<double> spectrumAnalyzerBands(octaveBands);
    SpectrumAnalyzerBands<double> sp;
    //spectrumAnalyzerBands[5200].sampleAmount = 100;
    //std::cout<<spectrumAnalyzerBands[7000].sampleAmount;
    sp = spectrumAnalyzerBands;
    //std::cout<<sp.getData()[0].sampleAmount;
}

void printXInterval(size_t b, interval<int> xInterval) {
    std::cout<< "["
             << "Bandwidth Designator: "<< b
             <<", Lower x: " << xInterval.lower()
             <<  ", Upper x: " << xInterval.upper()
             << ", Band amount: " << xInterval.upper() - xInterval.lower() + 1
             << "]"
             << std::endl;
}

void printEdgeFrequencies(size_t b, int x) {
    double G = FrequencyCalculator::getG<double>();
    //Fm : Frequency of mid-band
    double exactFm = FrequencyCalculator::calculateExactMidBandFrequency<double>(b, G, fRef, x);
    double nominalFm = FrequencyCalculator::calculateNominalFrequency<double>(b, exactFm);
    double lowerEdge = FrequencyCalculator::calculateLowerEdgeBandFrequency(b, G, exactFm);
    double upperEdge = FrequencyCalculator::calculateUpperEdgeBandFrequency(b, G, exactFm);
    std::cout << "X = " << x;
    std::cout << ", Nominal Fm = " << nominalFm;
    std::cout << ", Exact Fm = " << exactFm;
    std::cout << ", Lower Edge = " << lowerEdge;
    std::cout << ", Upper Edge = " << upperEdge;
    std::cout << std::endl;
}

void testXIntervals(size_t b) {
    interval<int> xInterval = FrequencyCalculator::calculateXInterval(b);

    printXInterval(b, xInterval);

    std::cout << "---- Lower Edge ----" << std::endl;
    printEdgeFrequencies(b, xInterval.lower());

    std::cout << "---- Upper Edge ----" << std::endl;
    printEdgeFrequencies(b, xInterval.upper());
}

void testRoundBy() {
    std::cout << std::endl <<AndromedaDSP::MathUtil::roundBy(156.256, 5) << std::endl;
}

void testNominalFrequency(size_t b) {
    std::cout<< FrequencyCalculator::calculateNominalFrequency(b, 41.567)<<std::endl;
}

int main() {
    testXIntervals(1);
    testXIntervals(2);
    testXIntervals(3);
    testXIntervals(24);

    return 0;
}
