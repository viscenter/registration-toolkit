#include <iostream>

#include <boost/filesystem.hpp>

#include "rt/io/ImageIO.hpp"
#include "rt/io/TIFFIO.hpp"
#include "rt/types/ITKOpenCVBridge.hpp"

namespace fs = boost::filesystem;

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << argv[0] << "Usage: input.tif output.tif" << std::endl;
        return EXIT_FAILURE;
    }

    // Get inputs
    fs::path input = argv[1];
    fs::path output = argv[2];

    // Load image
    auto img = rt::io::ReadRawTIFF(input, 10);

    // Save image
    rt::WriteImage(output, img);
}
