#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include "rt/DisegniSegmenter.hpp"

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    ///// Parse the command line options /////
    // clang-format off
    po::options_description required("General Options");
    required.add_options()
        ("help,h", "Show this message")
        ("input-image,i", po::value<std::string>()->required(), "Input disegni image")
        ("output-dir,o", po::value<std::string>()->required(),
         "Output directory segmented disegni images");

    po::options_description all("Usage");
    all.add(required);
    // clang-format on

    // Parse the cmd line
    po::variables_map parsed;
    po::store(po::command_line_parser(argc, argv).options(all).run(), parsed);

    // Show the help message
    if (parsed.count("help") || argc < 4) {
        std::cerr << all << std::endl;
        return EXIT_SUCCESS;
    }

    // Warn of missing options
    try {
        po::notify(parsed);
    } catch (po::error& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Load input
    fs::path inputPath = parsed["input-image"].as<std::string>();
    auto input = cv::imread(inputPath.string());
    if (input.empty()) {
        std::cout << "Could not open or find the image" << std::endl;
        return EXIT_FAILURE;
    }

    rt::DisegniSegmenter segmenter;
    segmenter.setInputImage(input);
    auto results = segmenter.compute();

    // TESTING PURPOSE
    cv::namedWindow("Fragmented Image", 0);
    cv::imshow("Fragmented Image", results[0]);
    cv::waitKey(0);
    return EXIT_SUCCESS;
    // TESTING PURPOSE
}
