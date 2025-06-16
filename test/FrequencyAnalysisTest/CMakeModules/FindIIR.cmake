# FindIIR
# ------------
# Finds the IIR as DSP IIR realtime filter library
#
# This will define the following variables:
#
# IIR_FOUND       - system has IIR
# IIR_INCLUDE_DIR - The IIR include directory
# IIR_LIBRARIES - The IIR libraries
#

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_IIR iir QUIET)
endif()

find_path(IIR_INCLUDE_DIR iir.h
        PATHS ${PC_IIR_INCLUDEDIR})
find_library(IIR_LIBRARY NAMES iir
        PATHS ${PC_IIR_LIBDIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(IIR REQUIRED_VARS IIR_INCLUDE_DIR PC_IIR_LIBDIR)

mark_as_advanced(IIR_INCLUDE_DIR IIR_LIBRARY)