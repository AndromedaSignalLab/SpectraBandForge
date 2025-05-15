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

enum OctaveBandBase{
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
