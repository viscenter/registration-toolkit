#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>

#include "rt/LandmarkReader.hpp"

using namespace rt;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* argv[])
{
    ///// Parse the command line options /////
    // clang-format off
    po::options_description required("General Options");
    required.add_options()
        ("help,h", "Show this message")
        ("moving,m", po::value<std::string>()->required(), "Moving image")
        ("fixed,f", po::value<std::string>()->required(), "Fixed image")
        ("output-file,o", po::value<std::string>()->required(),
            "Output file path for the registered moving image")
        ("output-tfm", po::value<std::string>(),
            "Output file path for the generated transform file");

    po::options_description ldmOptions("Landmark Registration Options");
    ldmOptions.add_options()
        ("landmarks,l", po::value<std::string>(),"Landmarks file")
        ("landmark-affine", "Limit landmark registration to affine transform");

    po::options_description deformOptions("Deformable Registration Options");
    deformOptions.add_options()
        ("deformable-iterations,i", po::value<int>()->default_value(100),
            "Number of deformable optimization iterations");

    po::options_description all("Usage");
    all.add(required).add(ldmOptions).add(deformOptions);
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

    fs::path fixedPath = parsed["fixed"].as<std::string>();
    fs::path movingPath = parsed["moving"].as<std::string>();
    fs::path outputPath = parsed["output-file"].as<std::string>();

    ///// Setup input files /////
    auto fixedImage = cv::imread(fixedPath.string(), -1);
    auto movingImage = cv::imread(movingPath.string(), -1);
    auto result = cv::Mat(fixedImage.rows, fixedImage.cols, movingImage.type());

    ///// Landmark Registration /////
    printf("Running landmark registration...\n");
    fs::path landmarksFileName = parsed["landmarks"].as<std::string>();
    LandmarkReader landmarkReader(landmarksFileName);
    landmarkReader.read();
    auto fixedLandmarks = landmarkReader.getFixedCVPoints();
    auto movingLandmarks = landmarkReader.getMovingCVPoints();

    auto T = cv::findHomography(movingLandmarks, fixedLandmarks, cv::RANSAC);

    cv::warpPerspective(movingImage, result, T, {result.cols, result.rows});

    cv::imwrite("landmark.png", result);

    ///// Deformable Registration /////
    printf("Running deformable registration...\n");

    // Dense Optical flow
    cv::Mat prev, next;
    fixedImage.convertTo(prev, CV_8U);
    cv::cvtColor(prev, prev, cv::COLOR_BGR2GRAY);
    result.convertTo(next, CV_8U, 255.0 / 65535.0);

    cv::Mat flow;
    cv::calcOpticalFlowFarneback(
        next, prev, flow, 0.5, 3, 20, 50, 7, 1.5,
        cv::OPTFLOW_FARNEBACK_GAUSSIAN);

    cv::Mat map(flow.size(), CV_32FC2);
    for (int y = 0; y < map.rows; ++y) {
        for (int x = 0; x < map.cols; ++x) {
            cv::Point2f f = flow.at<cv::Point2f>(y, x);
            map.at<cv::Point2f>(y, x) = cv::Point2f(x + f.x, y + f.y);
        }
    }

    printf("Remapping image...\n");
    cv::Mat result2;
    cv::remap(result, result2, map, cv::Mat(), cv::INTER_CUBIC);
    cv::imwrite("flow.png", result);

    return EXIT_SUCCESS;
}
