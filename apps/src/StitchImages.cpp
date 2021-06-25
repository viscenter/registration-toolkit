#include <exception>
#include <iostream>
#include <vector>

#include <boost/program_options.hpp>
#include <opencv2/core.hpp>

#include "rt/ImageStitcher.hpp"
#include "rt/filesystem.hpp"
#include "rt/io/ImageIO.hpp"
#include "rt/util/ImageConversion.hpp"

namespace fs = rt::filesystem;
namespace po = boost::program_options;

using LandmarkMode = rt::ImageStitcher::LandmarkMode;
using LandmarkPair = rt::ImageStitcher::LandmarkPair;

// std::vector<LandmarkPair> getLandmarks(std::vector<std::string> ldmFiles){
//    std::vector<rt::ImageStitcher::LandmarkPair> landmarks;
//    for(int i = 0; i < ldmFiles.size(); i++){
//        rt::ImageStitcher::LandmarkPair ldm;
//        ldm.srcIdx = i;
//        ldm.dstIdx = i+1;
//        std::ifstream landmarkFile;
//        // Open the landmarks file
//        landmarkFile.open(ldmFiles[i]);
//        // Check that the landmarks file opened
//        if (!landmarkFile.is_open()) {
//            throw std::runtime_error(
//                    "Could not open landmarks file " + ldmFiles[i] + ".");
//        }
//
//        std::pair<float, float> point1;
//        std::pair<float, float> point2;
//        // Read in the landmarks and store them
//        landmarkFile >> point1.first;
//        while (!landmarkFile.eof()) {
//            landmarkFile >> point1.second;
//            ldm.srcLdms.push_back(point1);
//            landmarkFile >> point2.first;
//            landmarkFile >> point2.second;
//            ldm.dstLdms.push_back(point2);
//            landmarkFile >> point1.first;
//        }
//        // Close the landmarks file
//        landmarkFile.close();
//        landmarks.push_back(ldm);
//    }
//    return landmarks;
//}

int main(int argc, char* argv[])
{
    // Adds the options that the user can set for the program
    // clang-format off
    po::options_description opts("General Options");
    opts.add_options()
        ("help,h", "Show this message")
        ("input-file,i", po::value<std::vector<std::string>>()->required(),
             "Image to be included in stitch. May be specified multiple times.")
        ("output-file,o", po::value<std::string>()->required(),
             "Path to the stitched output image")
        ("ldm-pair,l", po::value<std::vector<std::string>>(),
             "Landmark pairs file. May be specified multiple times.")
        ("ldm-mode", po::value<int>()->default_value(0), "Determines how the "
             "stitcher will use provided landmarks: \n"
             "\t0: Automatic - Provided landmarks will be ignored.\n"
             "\t1: Manual - Only provided landmarks will be used.\n"
             "\t2: Pre-match - Provided landmarks are included for potential matching.\n"
             "\t3: Post-match - Provided matches are included before best match filtering.\n"
             "\t4: Fallback - For each image pair, provided matches are used if no better matches are found.");
    // clang-format on

    // Sets the program options
    po::options_description all("Usage");
    all.add(opts);

    // Gets what options the user set
    po::variables_map parsed;
    po::store(po::command_line_parser(argc, argv).options(all).run(), parsed);

    // Checks if the user asked for help or if there are not enough arguments
    // given
    if (parsed.count("help") || argc < 2) {
        std::cerr << all << std::endl;
        return EXIT_FAILURE;
    }

    // Warn of missing options
    try {
        po::notify(parsed);
    } catch (po::error& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Get options
    auto inputPaths = parsed["input-file"].as<std::vector<std::string>>();
    fs::path outputPath = parsed["output-file"].as<std::string>();
    std::vector<std::string> ldmPaths;
    if (parsed.count("ldm-pair")) {
        ldmPaths = parsed["ldm-pair"].as<std::vector<std::string>>();
    }
    auto ldmMode = static_cast<LandmarkMode>(parsed["ldm-mode"].as<int>());

    // Load images
    std::cout << "Loading images..." << std::endl;
    std::vector<cv::Mat> imgs;
    for (const auto& p : inputPaths) {
        auto i = rt::ReadImage(p);
        if (i.empty()) {
            std::cerr << "Error: Failed to load image: " + p << std::endl;
            return EXIT_FAILURE;
        }
        i = rt::QuantizeImage(i, CV_8U);
        i = rt::ColorConvertImage(i, 3);
        imgs.emplace_back(i);
    }

    // Load landmarks
    // TODO
    std::vector<LandmarkPair> ldms;

    // Perform stitching
    std::cout << "Stitching images..." << std::endl;
    rt::ImageStitcher stitcher;
    stitcher.setImages(imgs);
    stitcher.setLandmarks(ldms);
    stitcher.setLandmarkMode(ldmMode);
    auto stitched = stitcher.compute();

    // Save image
    std::cout << "Saving stitched image..." << std::endl;
    rt::WriteImage(outputPath, stitched);
    std::cout << "Done." << std::endl;
}