# registration-toolkit

[![Build and Test](https://github.com/viscenter/registration-toolkit/workflows/Build%20and%20Test/badge.svg)](https://github.com/viscenter/registration-toolkit/actions)

A set of utilities written in C++ for simplifying image alignment tasks. 
Supports image-to-image and image-to-mesh registration across a range of image 
depths and types. Fully supports gray, gray+alpha, RGB, and RGBA images in 8, 
16, and 32 bits-per-channel.

## Requirements
* C++14 compiler
* Boost 1.58+
    - Required: Program Options
    - Optional: Filesystem - This project will automatically check if the 
      compiler provides `std::filesystem`. If it is not found, then 
      [Boost.Filesystem](https://www.boost.org/) is required. This behavior 
      can be controlled with the `RT_USE_BOOSTFS` CMake flag.
* OpenCV 4+
* ITK 4+
* VTK 6+
* libtiff 4.0.9+
    - **Note**: ITK and OpenCV should be linked against the same libtiff build
    
### Ubuntu 20.04
Install using apt:
```shell
sudo apt-get install cmake libopencv-dev libvtk6-dev libboost-program-options-dev libinsighttoolkit4-dev
```

### macOS
Install using Homebrew and the provided Brewfile:
```bash
cd registration-toolkit/
brew bundle
```

### vc-deps
Our research group maintains a CMake project called 
[vc-deps](https://gitlab.com/educelab/vc-deps) for building dependencies that 
are common across many of our C++ projects. Once it is built, point this 
project's CMake configuration at the `vc-deps` libraries using the 
`CMAKE_PREFIX_PATH` flag:

```shell
cmake -DCMAKE_PREFIX_PATH=/path/to/vc-deps/deps/ ..
```

## Build and Install
This project uses a CMake build system and can be built using the default CMake 
workflow:

```shell
# Get the source code 
git clone https://github.com/viscenter/registration-toolkit.git
cd registration-toolkit/

# Make an out-of-source build directory
mkdir build/ 
cd build/

# Configure and build
cmake ..
make
make install # optional
```

The CMake project provides a number of flags for configuring the build:
* `RT_BUILD_APPS`: Compile the utility applications. (Default: ON)
* `RT_BUILD_DOCS`: Build documentation. Dependencies: Doxygen, Graphviz 
  (optional). (Default: ON if Doxygen is found)
* `RT_INSTALL_DOCS`: Install HTML documentation to the system. (Default: OFF)
* `RT_BUILD_TESTS`: Build project unit tests. This will download and build the 
  Google Test framework. (Default: OFF)
* `RT_USE_BOOSTFS`: Use the `Boost::filesystem` library instead of 
  `std::filesystem`. (Default: ON if `std::filesystem` is not found)
* `RT_USE_VOLCART`: Build with optional Volume Cartographer components 
  (Default: OFF)
  
Flags can be set using `ccmake` or by providing them at configuration time. 
Example:
```shell
cmake -DRT_BUILD_TESTS=ON ..
```

## Usage
### Image-to-Image Registration
To align a moving image `close-up.jpg` to a fixed image `wide-angle.jpg`:

```shell
rt_register2d -f wide-angle.jpg -m close-up.jpg -o result.jpg
```

**Note:** By default, this application will attempt to automatically detect and 
match features between the two images in order to perform registration. To 
provide pre-computed landmarks, please provide a [Landmarks file](#Landmarks-files) using the 
`--input-landmarks` flag.

### Image-to-3D Mesh Registration
To align a moving image `color-photo.jpg` to a textured 3D mesh `grayscale-mesh.obj`:

```shell
rt_register3d -f color-photo.jpg -m grayscale-mesh.obj -o color-mesh.obj
```

**Note:** This process uses a 2D-to-2D registration process between the moving 
image and the texture image provided by the mesh file. This assumes that the 
provided mesh is roughly planar and has a "coherent" UV map (i.e. a single, 
continuous chart). Use `rt_reorder_texture` to convert a multi-chart mesh to a 
single chart one.

### Utilities:
* `rt_apply_transform`: Apply a Transform produced by `rt_register2d` or 
  `rt_register3d` to an image. Useful for duplicating exact registration 
  results.
* `rt_generate_landmarks`: Automatically detect and generate landmarks between 
  two images and save as a [Landmarks file](#Landmarks-files).
* `rt_swap_landmarks`: Swap the order (i.e. fixed <-> moving) of an existing 
  Landmarks file.
* `rt_plot_landmarks`: Plot a Landmarks file on the fixed and moving images.
* `rt_retexture_mesh`: Re-save a mesh with a new texture image. Does not modify 
  the UV map, so assumes that the replacement image has already been aligned to 
  the current texture image.
* `rt_segment_disegni`: Separate a composite disegni image into individual 
  pieces. More information coming soon.

### Landmarks files
A Landmarks file is a space-separated plain-text document where each line 
represents a pair of matching pixel positions in the fixed and moving images. 
Pairs are in the format `fX fY mX mY`. Values are interpreted as floating point 
value strings using `std::stod`. The `#` character begins a comment which 
terminates at the end of the line:

``` shell
# My Image Landmarks
500 100 2500 1000 # Integer
101.56 234.56 1010.911 2345.67 # Decimal float
1.0156e02 2.3456e02 1.010911e03 2.34567e03 # Scientific
```

## Publications
Parsons, Stephen, C. Seth Parker, and W. Brent Seales. "The St. Chad Gospels: Diachronic Manuscript Registration and Visualization." _Manuscript Studies: A Journal of the Schoenberg Institute for Manuscript Studies_ 2.2 (2017): 483-498.