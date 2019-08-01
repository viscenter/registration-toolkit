#include <fstream>
#include <iostream>
#include <vector>

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "rt/LandmarkDetector.hpp"

namespace fs = boost::filesystem;

void WriteLDM(const fs::path& path, const std::vector<rt::LandmarkPair>& pairs);

int main(int argc, const char* argv[])
{
    // Check arg count
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " [fixed] [moving] [output]"
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Get the paths
    fs::path fixedPath = argv[1];
    fs::path movingPath = argv[2];
    fs::path outputPath = argv[3];

    // Load images
    auto fixedImg = cv::imread(fixedPath.string());
    auto movingImg = cv::imread(movingPath.string());

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
    auto matchedPairs = detector.compute();

    // Write the output
    WriteLDM(outputPath, matchedPairs);

    return EXIT_SUCCESS;
}

void WriteLDM(const fs::path& path, const std::vector<rt::LandmarkPair>& pairs)
{
    std::ofstream ofs(path.string());
    if (!ofs.good()) {
        auto msg = "Failed to open file for writing: " + path.string();
        throw std::runtime_error(msg);
    }

    // Write the data
    for (auto& p : pairs) {
        // Fixed
        ofs << p.first.x << " " << p.first.y << " ";
        // Moving
        ofs << p.second.x << " " << p.second.y;
        ofs << std::endl;
    }

    // Close the file
    ofs.close();
}