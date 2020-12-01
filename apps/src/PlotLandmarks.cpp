#include <iostream>

#include <boost/program_options.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "rt/filesystem.hpp"
#include "rt/io/ImageIO.hpp"
#include "rt/io/LandmarkIO.hpp"

using namespace rt;

namespace po = boost::program_options;
namespace fs = rt::filesystem;

int main(int argc, char* argv[])
{
    ///// Parse the command line options /////
    // clang-format off
    po::options_description required("General Options");
    required.add_options()
        ("help,h", "Show this message")
        ("moving,m", po::value<std::string>()->required(), "Moving image")
        ("fixed,f", po::value<std::string>()->required(), "Fixed image");

    po::options_description ldmOptions("Landmark Registration Options");
    ldmOptions.add_options()
        ("landmarks,l", po::value<std::string>()->required(),
         "Landmarks file");

    po::options_description all("Usage");
    all.add(required).add(ldmOptions);
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
    fs::path landmarksFileName = parsed["landmarks"].as<std::string>();

    ///// Setup input files /////
    // Load the fixed and moving image at 8bpc
    auto fixed = cv::imread(fixedPath.string());
    auto moving = cv::imread(movingPath.string());

    // Load the landmarks
    LandmarkReader landmarkReader(landmarksFileName);
    landmarkReader.read();
    auto fixedLandmarks = landmarkReader.getFixedLandmarks();
    auto movingLandmarks = landmarkReader.getMovingLandmarks();

    if (fixedLandmarks.size() != movingLandmarks.size()) {
        std::cerr << "ERROR: Unequal number of landmarks" << std::endl;
        return EXIT_FAILURE;
    }

    for (const auto& p : fixedLandmarks) {
        cv::Point2f pt{static_cast<float>(p[0]), static_cast<float>(p[1])};
        cv::circle(fixed, pt, 10, {0, 255, 0}, -1);
    }

    for (const auto& p : movingLandmarks) {
        cv::Point2f pt{static_cast<float>(p[0]), static_cast<float>(p[1])};
        cv::circle(fixed, pt, 10, {0, 255, 0}, -1);
    }

    fs::path fixedPlot = fixedPath.stem().string() + "_plot.png";
    rt::WriteImage(fixedPlot, fixed);

    fs::path movingPlot = movingPath.stem().string() + "_plot.png";
    rt::WriteImage(movingPlot, moving);
}
