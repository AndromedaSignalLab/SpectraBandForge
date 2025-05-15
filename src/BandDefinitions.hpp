/*
Band definitions of SpectraBandForge project
Copyright (C) 2025 Volkan Orhan

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#pragma once
#include <cstddef>

#define fRef 1000

template <class T>
struct NominalFrequency {
    int indexX;
    T nominalMidBandFrequency;
    T exactMidBandFrequency;
};

enum OctaveBandBase {
    Base2 = 2,
    Base10 = 10
};

template <class T>
struct OctaveBand {
    size_t bandNumber;
    OctaveBandBase base;
    T midBandFrequency;
    T nominalMidBandFrequency;
    T upperEdgeBandFrequency;
    T lowerEdgeBandFrequency;
};

//https://www.nti-audio.com/Portals/0/data/en/1-n-Octave-band-filter-data.pdf

static const NominalFrequency<double> nominalFrequenciesOfThirdOctaveBands[] = {
    //Calculated values
    {-31, 0.8, 0.794},
    {-30, 1, 1},
    {-29, 1.25, 1.259},
    {-28, 1.6, 1.585},
    {-27, 2, 1.995},
    {-26, 2.5, 2.512},
    {-25, 3.15, 3.162},
    {-24, 4, 3.98},
    {-23, 5, 5.012},
    {-22, 6.3, 6.310},
    {-21, 8, 7.943},
    {-20, 10, 10},
    {-19, 12.5, 12.59},
    {-18, 16, 15.85},
    {-17, 20, 19.953},
    //Table values
    {-16, 25, 25.119},
    {-15, 31.5, 31.623},
    {-14, 40, 39.811},
    {-13, 50, 50.119},
    {-12, 63, 63.096},
    {-11, 80, 79.433},
    {-10, 100, 100.00},
    {-9, 125, 125.89},
    {-8, 160, 158.49},
    {-7, 200, 199.53},
    {-6, 250, 251.19},
    {-5, 315, 316.23},
    {-4, 400, 398.11},
    {-3, 500, 501.19},
    {-2, 630, 630.96},
    {-1, 800, 794.33},
    {0, 1000, 1000.0},
    {1, 1250, 1258.9},
    {2, 1600, 1584.9},
    {3, 2000, 1995.3},
    {4, 2500, 2511.9},
    {5, 3150, 3162.3},
    {6, 4000, 3981.1},
    {7, 5000, 5011.9},
    {8, 6300, 6309.6},
    {9, 8000, 7943.3},
    {10, 10000, 10000},
    {11, 12500, 12589},
    {12, 16000, 15849},
    {13, 20000, 19953}
};

static const NominalFrequency<double> nominalFrequenciesOfFirstOctaveBands[] = {
    //Calculated values
    {-10, 1, 1},
    {-9, 2, 1.995},
    {-8, 4, 3.98},
    {-7, 8, 7.943},
    {-6, 16, 15.85},
    //Table values
    {-5, 31.5, 31.623},
    {-4, 63, 63.096},
    {-3, 125, 125.89},
    {-2, 250, 251.19},
    {-1, 500, 501.19},
    {0, 1000, 1000.0},
    {1, 2000, 1995.3},
    {2, 4000, 3981.1},
    {3, 8000, 7943.3},
    {4, 16000, 15849}
};

const double NominalFrequencies[] = {
    0.8, 1, 1.25, 1.6, 2, 2.5, 3.15, 4, 5, 6.3, 8, 10,
    12.5, 16, 20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160,
    200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500,
    3150, 4000, 5000, 6300, 8000, 10000, 12500, 16000, 20000
};
