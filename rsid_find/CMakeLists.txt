cmake_minimum_required( VERSION 3.6 )

# Require C++17 (or later)
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

# Create Project
project( rsid_find LANGUAGES CXX )
add_executable( rsid_find RealSenseID/FindDevices.h main.cpp )

# Set StartUp Project
set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT "rsid_find" )

set( REALSENSEIDSDK_DIR "$ENV{ProgramFiles}/Intel RealSenseID SDK" )
include_directories( ${REALSENSEIDSDK_DIR}/include )
target_link_libraries( rsid_find ${REALSENSEIDSDK_DIR}/lib/rsid.lib )