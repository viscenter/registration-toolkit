#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <itkCompositeTransform.h>
#include <itkOpenCVImageBridge.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "rt/AffineLandmarkRegistration.hpp"
#include "rt/BSplineLandmarkWarping.hpp"
#include "rt/DeformableRegistration.hpp"
#include "rt/ImageTransformResampler.hpp"
#include "rt/ImageTypes.hpp"
#include "rt/io/LandmarkReader.hpp"

using namespace rt;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using OCVB = itk::OpenCVImageBridge;

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
    auto cvFixed = cv::imread(fixedPath.string());
    auto fixedImage = OCVB::CVMatToITKImage<Image8UC3>(cvFixed);
    auto cvMoving = cv::imread(movingPath.string());
    auto movingImage = OCVB::CVMatToITKImage<Image8UC3>(cvMoving);

    // Ignore spacing information
    fixedImage->SetSpacing(1.0);
    movingImage->SetSpacing(1.0);

    LandmarkReader landmarkReader(landmarksFileName);
    landmarkReader.setFixedImage(fixedImage);
    landmarkReader.setMovingImage(movingImage);
    landmarkReader.read();
    auto fixedLandmarks = landmarkReader.getFixedLandmarks();
    auto movingLandmarks = landmarkReader.getMovingLandmarks();

    if (fixedLandmarks.size() != movingLandmarks.size()) {
        std::cerr << "ERROR: Unequal number of landmarks" << std::endl;
        return EXIT_FAILURE;
    }

    for (const auto& p : fixedLandmarks) {
        cv::Point2f pt{static_cast<float>(p[0]), static_cast<float>(p[1])};
        cv::circle(cvFixed, pt, 10, {0, 255, 0}, -1);
    }

    for (const auto& p : movingLandmarks) {
        cv::Point2f pt{static_cast<float>(p[0]), static_cast<float>(p[1])};
        cv::circle(cvFixed, pt, 10, {0, 255, 0}, -1);
    }

    fs::path fixedPlot = fixedPath.stem().string() + "_plot.png";
    cv::imwrite(fixedPlot.string(), cvFixed);

    fs::path movingPlot = movingPath.stem().string() + "_plot.png";
    cv::imwrite(movingPlot.string(), cvMoving);
}
