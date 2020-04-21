#include <iostream>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "rt/ImageStitcher.hpp"


namespace fs = boost::filesystem;
namespace po = boost::program_options;

auto divide_images = false;

int main(int argc, char* argv[])
{
    // Adds the options that the user can set for the program
    po::options_description stitching("Stitching Options");
    stitching.add_options()
            ("help,h", "Show this message")
            ("images,imgs", po::value<std::vector<std::string> >()->multitoken()->required(), "Images to be stitched")
            ("output-file,o", po::value<std::string>()->required(),
             "Output image")
            ("input-ldm,ldm", po::value<std::vector<std::string> >()->multitoken(), "User generated landmarks file")
            ("option", po::value<int>(), "Option 1, 2, or 3 for storing the user generated landmarks")
            ("disable-auto-ldm", "Disable automatically generated landmarks");

    // Sets the program options
    po::options_description all("Usage");
    all.add(stitching);

    // Gets what options the user set
    po::variables_map parsed;
    po::store(po::command_line_parser(argc, argv).options(all).run(), parsed);

    // Checks if the user asked for help or if there are not enough arguments given
    if(parsed.count("help") || argc < 3){
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
        std::cerr << "Cannot disable automatic landmark generation and not provide landmarks." << std::endl;
        return EXIT_FAILURE;
    }

    int option = 2;
    if(parsed.count("option")) {
        option = parsed["option"].as<int>();
    }

    if(option < 1 || option > 3){
        std::cerr << "Option values can only be 1, 2, or 3 when giving user generated landmarks." << std::endl;
        return EXIT_FAILURE;
    }

    // Get the output file path and the paths of the images
    fs::path output = parsed["output-file"].as<std::string>();
    std::vector<std::string> images = parsed["images"].as<std::vector<std::string>>();

    // Read in the images and store them
    std::vector<cv::Mat> imgs;
    for (int i = 0; i < images.size(); i++) {
        cv::Mat img = cv::imread(images[i]);
        /*if (divide_images) {
            cv::Rect rect(0, 0, img.cols / 2, img.rows);
            imgs.push_back(img(rect).clone());
            rect.x = img.cols / 3;
            imgs.push_back(img(rect).clone());
            rect.x = img.cols / 2;
            imgs.push_back(img(rect).clone());
        } else {
            imgs.push_back(img);
        }*/
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
        std::vector<std::string> ldmFiles = parsed["input-ldm"].as<std::vector<std::string>>();

        // Set the user specified landmarks for the stitcher
        stitcher.setLandmarks(ldmFiles);
    }

    // Set the bool that tells the program if landmarks should automatically be generated
    stitcher.setGenerateLandmarks(!parsed.count("disable-auto-ldm"));

    stitcher.setOption(option);

    // Stitch the images together
    auto res = stitcher.compute();

    // Write the stitched image to the output file specified by the user
    cv::imwrite(output.string(), res);
}