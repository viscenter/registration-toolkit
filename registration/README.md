registration
====================

LandmarkRegistration
--------------------

Program to use for image registration. Meant to replace the use of ITK examples LandmarkWarping2 and DeformableRegistration13.

To build:
- create build directory ```mkdir bin/```
- move to build directory ```cd bin/```
- run cmake ```ccmake ..```
- press c to configure
- press g to generate Makefile
- press q to exit ccmake
- make the executables ```make```

Usage: LandmarkRegistration landmarksFile fixedImage movingImage outputImageFile transformDestination numberOfIterations [createVideoFrames]

- landmarksFile:        Text file with lines of the format '''a b c d''' where pixel index (a, b) on the static image corresponds to (c, d) on the moving image.
- fixedImage:           Image to be unchanged in registration.
- movingImage:          Image to be aligned to the fixed image.
- outputImageFile:      After the moving image is warped to align with the fixed image, the resulting warped image is saved with this name.
- transformDestination: Path and filename of the transform file which is created after registration and holds the transformation information so that the work can be reproduced without computation.
- numberOfIterations:   Desired maximum number of iterations. Sometimes the registration process will stop before this number if the registration has converged.
- [createVideoFrames]:  If anything is given here, the program will save an image for each registration iteration. The resulting group of images can be put into a video to better view the registration process.

ApplyTransform
--------------

Usage: ApplyTransform transformationFile movingImage outputImage

- transformationFile: File output from LandmarkRegistration. Contains transform values as well as information regarding the fixed image size and spacing.
- movingImage:        Image to be transformed. The resulting transformed image will be registered to the fixed image as it was after LandmarkRegistration, but here we can avoid the computation necessary to carry out registration again.
- outputImage:        The transformed moving image is saved here.

Known Issues
------------

- LandmarkRegistration does not function exactly the same on large and small images. It works on both, but the results are not _exactly_ the same as they should be. This might be due to a misunderstanding on our part of the optimizer and how it is currently programmed to account for differences in image sizes.
- The bit depth is currently hardcoded in LandmarkRegistration. This should be changed so that the RGB pixels have the same bit depth as the moving image and the gray pixels have the same bit depth as the static image.
- The registration is not yet perfect. On some images it is near perfect but on others it seems it should be slightly more "flexible". Sometimes the corners just don't quite get there.