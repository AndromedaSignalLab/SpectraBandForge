#Build script for SpectrumAnalysisTest of SpectraBandForge Project
#Copyright (C) 2025 Volkan Orhan

#This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

#This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

#You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

cmake_minimum_required(VERSION 3.16)

project(FrequencyAnalysisTest VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_language(C)

#Boost Definitions
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

if(APPLE)
    set(CMAKE_MACOSX_BUNDLE ON)
    set(OpenMPT_USE_STATIC_LIBS false)
    set(FFTW_USE_STATIC_LIBS true)
    #set(BOOST_USE_STATIC_LIBS true)
    #set(FFTW_USE_STATIC_LIBS true)
    set(PortAudio_USE_STATIC_LIBS true)
    set(PortAudioCPP_USE_STATIC_LIBS true)
endif(APPLE)

set(COMPONENTS_ROOT_PATH ${PROJECT_SOURCE_DIR}/../../..)
set(CMAKE_MODULE_PATH ${COMPONENTS_ROOT_PATH}/AndromedaCommon/CMakeModules)

if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Boost REQUIRED)
find_package(PortAudio REQUIRED)
find_package(PortAudioCPP REQUIRED)
find_package(LibSamplerate REQUIRED)
find_package(FFTW REQUIRED COMPONENTS
        DOUBLE_LIB
        FLOAT_LIB
        DOUBLE_THREADS_LIB
        FLOAT_THREADS_LIB
)
find_package(IIR REQUIRED)
find_package(spdlog REQUIRED)

include_directories(../../src)
include_directories(${COMPONENTS_ROOT_PATH}/AndromedaCommon/src)
include_directories(${COMPONENTS_ROOT_PATH}/AndromedaDSP/src)
include_directories(${COMPONENTS_ROOT_PATH}/KissFFT)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PORTAUDIO_INCLUDE_DIRS})
include_directories(${PORTAUDIOCPP_INCLUDE_DIRS})
include_directories(${FFTW_INCLUDES})
include_directories(${IIR_INCLUDE_DIR})

file(GLOB SpectraBandForgeSrc
    "../../src/*.hpp"
    "../../src/*.cpp"
    "../../src/Implementations/FFT/*.hpp"
    "../../src/Implementations/FFT/*.cpp"
)

file(GLOB AndromedaCommonSrc
        "${COMPONENTS_ROOT_PATH}/AndromedaCommon/src/*.hpp"
        "${COMPONENTS_ROOT_PATH}/AndromedaCommon/src/*.cpp"
)

file(GLOB AndromedaDspSrc
        "${COMPONENTS_ROOT_PATH}/AndromedaDSP/src/*.cpp"
        "${COMPONENTS_ROOT_PATH}/AndromedaDSP/src/*.hpp"
)

file(GLOB KissFFTSrc
        "${COMPONENTS_ROOT_PATH}/KissFFT/*.c"
        "${COMPONENTS_ROOT_PATH}/KissFFT/*.h"
)

file(GLOB FrequencyAnalysisTestSrc
    "./*.cpp"
    "./*.hpp"
    "./*.ui"
    "./*.qrc"
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(FrequencyAnalysisTest
        MANUAL_FINALIZATION
        ${FrequencyAnalysisTestSrc}
        ${SpectraBandForgeSrc}
        ${AndromedaCommonSrc}
        ${AndromedaDspSrc}
        ${KissFFTSrc}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET FrequencyAnalysisTest APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(FrequencyAnalysisTest SHARED
            ${FrequencyAnalysisTestSrc}
            ${SpectraBandForgeSrc}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(FrequencyAnalysisTest
            ${FrequencyAnalysisTestSrc}
            ${SpectraBandForgeSrc}
        )
    endif()
endif()

find_library(FFTW3_LIB NAMES fftw3)
find_library(FFTW3F_LIB NAMES fftw3f)
find_library(FFTW3L_LIB fftw3l)

if(NOT FFTW3_LIB OR NOT FFTW3F_LIB)
    message(FATAL_ERROR "FFTW not found")
endif()

target_link_libraries(FrequencyAnalysisTest PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        ${Boost_LIBRARIES}
        ${PORTAUDIO_LIBRARIES}
        ${PORTAUDIOCPP_LIBRARIES}
        ${LIBSAMPLERATE_LIBRARY}
        ${FFTW3_LIB}
        ${FFTW3F_LIB}
        ${FFTW3L_LIB}
        ${IIR_LIBRARY}
        spdlog::spdlog
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER org.AndromedaSignalLab.SpectraBandForge.FrequencyAnalysisTest)
endif()
set_target_properties(FrequencyAnalysisTest PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS FrequencyAnalysisTest
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FrequencyAnalysisTest)
endif()
