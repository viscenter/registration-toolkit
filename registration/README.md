registration
====================

Program to use for image registration. Meant to replace the use of ITK examples LandmarkWarping2 and DeformableRegistration13. Will use only the most recent version of ITK. ITK must be installed to build, but build should run as standalone executable.

To build:
- create build directory ```mkdir bin/```
- move to build directory ```cd bin/```
- run cmake ```ccmake ..```
- press c to configure
- press g to generate Makefile
- make the executables ```make```