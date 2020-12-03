#include <iostream>
#include <vector>
#include <exception>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "rt/ImageStitcher.hpp"

namespace fs = boost::filesystem;
namespace po = boost::program_options;

auto divide_images = false;

std::vector<rt::ImageStitcher::LandmarkPair> getLandmarks(std::vector<std::string> ldmFiles){
    std::vector<rt::ImageStitcher::LandmarkPair> landmarks;
    for(int i = 0; i < ldmFiles.size(); i++){
        rt::ImageStitcher::LandmarkPair ldm;
        ldm.srcIdx = i;
        ldm.dstIdx = i+1;
        std::ifstream landmarkFile;
        // Open the landmarks file
        landmarkFile.open(ldmFiles[i]);
        // Check that the landmarks file opened
        if (!landmarkFile.is_open()) {
            throw std::runtime_error(
                    "Could not open landmarks file " + ldmFiles[i] + ".");
        }

        std::pair<float, float> point1;
        std::pair<float, float> point2;
        // Read in the landmarks and store them
        landmarkFile >> point1.first;
        while (!landmarkFile.eof()) {
            landmarkFile >> point1.second;
            ldm.srcLdms.push_back(point1);
            landmarkFile >> point2.first;
            landmarkFile >> point2.second;
            ldm.dstLdms.push_back(point2);
            landmarkFile >> point1.first;
        }
        // Close the landmarks file
        landmarkFile.close();
        landmarks.push_back(ldm);
    }
    return landmarks;
}

int main(int argc, char* argv[])
{
    // Adds the options that the user can set for the program
    po::options_description stitching("Stitching Options");
    stitching.add_options()
            ("help,h", "Show this message")
            ("images,i", po::value<std::vector<std::string> >()->required(), "Images to be stitched")
            ("output-file,o", po::value<std::string>()->required(),
             "Output image")
            ("input-ldm,l", po::value<std::vector<std::string> >(), "User generated landmarks file")
            ("insert-ldm-pos", po::value<int>()->default_value(2), "The position to insert the user generated landmarks.\n"
                                                                   "1 -> After automatically generating matches\n"
                                                                   "2 -> After filtering out images with no matches\n"
                                                                   "3 -> Only as a fallback option if there are no matches for an image")
            ("disable-auto-ldm", "Disable automatically generated landmarks");

    // Sets the program options
    po::options_description all("Usage");
    all.add(stitching);

    // Gets what options the user set
    po::variables_map parsed;
    po::store(po::command_line_parser(argc, argv).options(all).run(), parsed);

    // Checks if the user asked for help or if there are not enough arguments given
    if(parsed.count("help") || argc < 7){
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

    // The user can't turn off automatically generated landmarks and not provide landmarks
    if(parsed.count("disable-auto-ldm") && !parsed.count("input-ldm")){
        std::cerr << "Error: Automatic landmarks disabled but manual landmarks not provided." << std::endl;
        return EXIT_FAILURE;
    }

    auto manualLdmPos = static_cast<rt::ImageStitcher::LandmarkMode>(
        parsed["insert-ldm-pos"].as<int>());

    if (manualLdmPos != rt::ImageStitcher::LandmarkMode::ManualPostMatch &&
        manualLdmPos != rt::ImageStitcher::LandmarkMode::ManualPreMatch &&
        manualLdmPos != rt::ImageStitcher::LandmarkMode::ManualFallback) {
        std::cerr << "Insert landmark position can only be 1, 2, or 3 when giving user generated landmarks." << std::endl;
        return EXIT_FAILURE;
    }

    // Get the output file path and the paths of the images
    fs::path output = parsed["output-file"].as<std::string>();
    auto imgPaths = parsed["images"].as<std::vector<std::string>>();

    // Read in the images and store them
    std::vector<cv::Mat> imgs;
    for (const auto& p : imgPaths) {
        // This will load the image as 8UC3
        cv::Mat img = cv::imread(p);
        imgs.push_back(img);
    }

    // Create the image stitcher and set the images
    rt::ImageStitcher stitcher;
    stitcher.setImages(imgs);

    // This will probably need to change to allow for user specified
    // landmarks for more than 2 images
    // Read in the landmarks if the user provides them
    if(parsed.count("input-ldm")) {
        // Get landmarks file paths
        auto ldmFiles = parsed["input-ldm"].as<std::vector<std::string>>();

        std::vector<rt::ImageStitcher::LandmarkPair> landmarks = getLandmarks(ldmFiles);

        // Set the user specified landmarks for the stitcher
        stitcher.setLandmarks(landmarks);
    }

    stitcher.setLandmarkMode(manualLdmPos);

    try {
        // Stitch the images together
        auto res = stitcher.compute();

        // Write the stitched image to the output file specified by the user
        cv::imwrite(output.string(), res);
    }
    catch (std::exception& e){
        std::cerr << e.what() << std::endl;
    }
}