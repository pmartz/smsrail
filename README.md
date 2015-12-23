# smsrail
Sandbox repo for computer controlled model railroad. For now, this is just for me to try out some ideas, but it might turn into a "real project" sometime in the future.


# Dependencies

This project depends on Boost, Doxygen and CMake.

Boost is required. Version 1.55 has been tested.

Doxygen is optional, for generating reference documentation. If
you don't have Doxygen installed on your dev system, CMake will
not create a build target for creating documentation. Doxygen
version 1.8.8 has been tested.

CMake is required for building. Version 3.0.2 has been tested.


# Build Instructions

The CMake scripts in this project use the following CMake
variables:

BOOST_ROOT - Boost installation directory.
BOOST_ALL_DYN_LINK - Use boost as dynamic/shared libraries.
CMAKE_INSTALL_PREFIX - Project installation directory.
SMSRAIL_DOCUMENTATION - Whether or not to create a documentation build target.

On any platform, you could run cmake-gui and fill in values for
these variables.

You can also run cmake on the command line. For example:

> cmake <pathToProjectRoot> -G "Visual Studio 10" \
    -DBOOST_ROOT:PATH=I:/boost -DBOOST_ALL_DYN_LINK:BOOL=ON \
    -DCMAKE_INSTALL_PREFIX:PATH=I:/rtw \
    -DSMSRAIL_DOCUMENTATION:BOOL=ON

Then open the rtw.sln file in Visual Studio and build as usual.

It's probably easier on Linux, as most dependencies are
installed into default locations:

> cmake <pathToProjectRoot> -G "Unix Makefiles" \
    -DBOOST_ALL_DYN_LINK:BOOL=ON \
    -DCMAKE_INSTALL_PREFIX:PATH=/usr/etc/smsrail \
    -DSMSRAIL_DOCUMENTATION:BOOL=ON

Then type "make" or "make install" as you desire.
