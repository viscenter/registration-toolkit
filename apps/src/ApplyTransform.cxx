#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <itkCompositeTransform.h>
#include <itkCompositeTransformIOHelper.h>
#include <itkTransformFactory.h>
#include <itkTransformFileReader.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "rt/SpatialObject.hpp"
#include "rt/ImageTransformResampler.hpp"

using CompositeTransform = itk::CompositeTransform<double, 2>;
using TransformConverter = itk::CompositeTransformIOHelperTemplate<double>;

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
            ("transform,t", po::value<std::string>()->required(),
             "Input file path for the transform file")
            ("output-file,o", po::value<std::string>()->required(),
             "Output file path for the registered moving image");

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

    fs::path fixedPath = parsed["fixed"].as<std::string>();
    fs::path movingPath = parsed["moving"].as<std::string>();
    fs::path tfmPath = parsed["transform"].as<std::string>();
    fs::path outputPath = parsed["output-file"].as<std::string>();

    // Register transforms
    itk::TransformFactoryBase::RegisterDefaultTransforms();

    // Read transform
    auto tfmReader = itk::TransformFileReader::New();
    tfmReader->SetFileName(tfmPath.string());
    tfmReader->Update();

    CompositeTransform::Pointer transform = dynamic_cast<CompositeTransform*>(
        tfmReader->GetTransformList()->begin()->GetPointer());

    // Load the moving image at full depth and resample it
    auto cvFixed = rt::SpatialObject::Load(fixedPath);
    auto cvMoving = rt::SpatialObject::Load(movingPath);
    cv::Mat cvFixedImg = cvFixed->getImage();
    cv::Mat cvMovingImg = cvMoving->getImage();
    cv::Size s(cvFixedImg.cols, cvFixedImg.rows);
    auto cvFinal = rt::ImageTransformResampler(cvMovingImg, s, transform);

    // Write out the file
    cv::imwrite(outputPath.string(), cvFinal);
}