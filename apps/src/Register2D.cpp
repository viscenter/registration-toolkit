#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <itkCompositeTransform.h>
#include <itkOpenCVImageBridge.h>
#include <itkTransformFileWriter.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "rt/AffineLandmarkRegistration.hpp"
#include "rt/BSplineLandmarkWarping.hpp"
#include "rt/DeformableRegistration.hpp"
#include "rt/ImageTransformResampler.hpp"
#include "rt/ImageTypes.hpp"
#include "rt/LandmarkReader.hpp"

using namespace rt;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

// Composite Transform
using CompositeTransform = itk::CompositeTransform<double, 2>;

// IO
using OCVB = itk::OpenCVImageBridge;
using TransformWriter = itk::TransformFileWriter;

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
        ("output-tfm,t", po::value<std::string>(),
            "Output file path for the generated transform file");

    po::options_description ldmOptions("Landmark Registration Options");
    ldmOptions.add_options()
        ("landmarks,l", po::value<std::string>(),"Landmarks file")
        ("landmark-disable-bspline", "Disable B-Spline Landmark Registration");

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
    // Load the fixed and moving image at 8bpc
    auto cvFixed = cv::imread(fixedPath.string());
    auto fixedImage = OCVB::CVMatToITKImage<Image8UC3>(cvFixed);
    auto cvMoving = cv::imread(movingPath.string());
    auto movingImage = OCVB::CVMatToITKImage<Image8UC3>(cvMoving);

    // Ignore spacing information
    fixedImage->SetSpacing(1.0);
    movingImage->SetSpacing(1.0);

    // Setup final transform
    auto compositeTrans = CompositeTransform::New();
    ///// Landmark Registration /////
    if (parsed.count("landmarks")) {
        printf("Running affine registration...\n");
        fs::path landmarksFileName = parsed["landmarks"].as<std::string>();
        LandmarkReader landmarkReader(landmarksFileName);
        landmarkReader.setFixedImage(fixedImage);
        landmarkReader.setMovingImage(movingImage);
        landmarkReader.read();
        auto fixedLandmarks = landmarkReader.getFixedLandmarks();
        auto movingLandmarks = landmarkReader.getMovingLandmarks();

        AffineLandmarkRegistration landmark;
        landmark.setFixedLandmarks(fixedLandmarks);
        landmark.setMovingLandmarks(movingLandmarks);
        auto ldmTransform = landmark.compute();
        compositeTrans->AddTransform(ldmTransform);

        // Resample moving image for next stage
        auto tmpMoving = ImageTransformResampler<Image8UC3>(
            movingImage, fixedImage->GetLargestPossibleRegion().GetSize(),
            compositeTrans);

        // Generate the landmark transform
        if (parsed.count("landmark-disable-bspline") == 0) {
            printf("Running B-spline landmark registration...\n");

            // Update the landmark positions
            auto i = compositeTrans->GetInverseTransform();
            for (auto& p : movingLandmarks) {
                p = i->TransformPoint(p);
            }

            // BSpline Warp
            BSplineLandmarkWarping bSplineLandmark;
            bSplineLandmark.setFixedImage(fixedImage);
            bSplineLandmark.setFixedLandmarks(fixedLandmarks);
            bSplineLandmark.setMovingLandmarks(movingLandmarks);
            auto warp = bSplineLandmark.compute();
            compositeTrans->AddTransform(warp);

            // Resample moving image for next stage
            tmpMoving = ImageTransformResampler<Image8UC3>(
                movingImage, fixedImage->GetLargestPossibleRegion().GetSize(),
                compositeTrans);
        }

        movingImage = tmpMoving;
    }

    ///// Deformable Registration /////
    auto iterations = parsed["deformable-iterations"].as<int>();
    if (iterations > 0) {
        printf("Running deformable registration...\n");
        rt::DeformableRegistration deformable;
        deformable.setFixedImage(fixedImage);
        deformable.setMovingImage(movingImage);
        deformable.setNumberOfIterations(iterations);
        auto deformTransform = deformable.compute();

        compositeTrans->AddTransform(deformTransform);
    }

    ///// Resample the source image /////
    printf("Resampling the moving image...\n");
    cvMoving = cv::imread(movingPath.string(), cv::IMREAD_UNCHANGED);
    cv::Size s(cvFixed.cols, cvFixed.rows);
    auto cvFinal = ImageTransformResampler(cvMoving, s, compositeTrans);

    ///// Write the output image /////
    printf("Writing output image to file...\n");
    cv::imwrite(outputPath.string(), cvFinal);

    ///// Write the final transformations /////
    if (parsed.count("output-tfm")) {
        fs::path transformFileName = parsed["output-tfm"].as<std::string>();
        printf("Writing transformation to file...\n");

        // Write deformable transform
        auto transformWriter = TransformWriter::New();
        transformWriter->SetFileName(transformFileName.string());
        transformWriter->SetInput(compositeTrans);
        transformWriter->Update();
    }

    return EXIT_SUCCESS;
}
