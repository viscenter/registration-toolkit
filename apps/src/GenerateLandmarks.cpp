#include <iostream>

#include "rt/LandmarkDetector.hpp"
#include "rt/filesystem.hpp"
#include "rt/io/ImageIO.hpp"
#include "rt/io/LandmarkIO.hpp"

namespace fs = rt::filesystem;

int main(int argc, const char* argv[])
{
    // Check arg count
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0]
                  << " [fixed] [moving] [output] {[conf]}" << std::endl;
        return EXIT_FAILURE;
    }

    // Get the paths
    fs::path fixedPath = argv[1];
    fs::path movingPath = argv[2];
    fs::path outputPath = argv[3];
    float confidence{0.3F};
    if (argc > 4) {
        confidence = std::stof(argv[4]);
    }

    // Load images
    auto fixedImg = rt::ReadImage(fixedPath);
    auto movingImg = rt::ReadImage(movingPath);

    // Check that images opened correctly
    if (fixedImg.empty() || movingImg.empty()) {
        std::cout << "Failed to read image(s)" << std::endl;
        return EXIT_FAILURE;
    }

    // Run matcher
    std::cout << "Matching: " << movingPath << " >> " << fixedPath << std::endl;
    rt::LandmarkDetector detector;
    detector.setFixedImage(fixedImg);
    detector.setMovingImage(movingImg);
    detector.setMatchRatio(confidence);
    auto matchedPairs = detector.compute().size();
    std::cout << "Generated matches: " << matchedPairs << std::endl;

    // Write the output
    std::cout << "Writing landmarks file..." << std::endl;
    rt::LandmarkWriter writer;
    writer.setPath(outputPath);
    writer.setFixedLandmarks(detector.getFixedLandmarks());
    writer.setMovingLandmarks(detector.getMovingLandmarks());
    writer.write();

    return EXIT_SUCCESS;
}