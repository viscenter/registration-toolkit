#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <itkCompositeTransform.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkTransformFileWriter.h>

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
using ImageReader = itk::ImageFileReader<Image8UC3>;
using ImageWriter = itk::ImageFileWriter<Image8UC3>;
using TransformWriter = itk::TransformFileWriterTemplate<double>;

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

    fs::path fixedImageFileName = parsed["fixed"].as<std::string>();
    fs::path movingImageFileName = parsed["moving"].as<std::string>();
    fs::path outputImageFileName = parsed["output-file"].as<std::string>();

    ///// Setup input files /////
    Image8UC3::Pointer fixedImage;
    Image8UC3::Pointer movingImage;
    Image8UC3::Pointer tmpMovingImage;

    // Read the two images
    try {
        // Read the fixed image
        auto imgReader = ImageReader::New();
        imgReader->SetFileName(fixedImageFileName.string());
        imgReader->Update();
        fixedImage = imgReader->GetOutput();

        // Read the moving image
        imgReader = ImageReader::New();
        imgReader->SetFileName(movingImageFileName.string());
        imgReader->Update();
        movingImage = imgReader->GetOutput();
    } catch (itk::ExceptionObject& excp) {
        std::cerr << "Exception thrown " << std::endl;
        std::cerr << excp << std::endl;
        return EXIT_FAILURE;
    }

    // Ignore spacing information
    fixedImage->SetSpacing(1.0);
    movingImage->SetSpacing(1.0);

    // Setup final transform
    auto compositeTrans = CompositeTransform::New();

    ///// Landmark Registration /////
    if (parsed.count("landmarks")) {
        printf("Running landmark registration...\n");
        fs::path landmarksFileName = parsed["landmarks"].as<std::string>();
        LandmarkReader landmarkReader(landmarksFileName);
        landmarkReader.setFixedImage(fixedImage);
        landmarkReader.setMovingImage(movingImage);
        landmarkReader.read();
        auto fixedLandmarks = landmarkReader.getFixedLandmarks();
        auto movingLandmarks = landmarkReader.getMovingLandmarks();

        // Generate the landmark transform
        if (parsed.count("landmark-affine")) {
            AffineLandmarkRegistration landmark;
            landmark.setFixedLandmarks(fixedLandmarks);
            landmark.setMovingLandmarks(movingLandmarks);
            auto ldmTransform = landmark.compute();
            compositeTrans->AddTransform(ldmTransform);
        } else {
            BSplineLandmarkWarping landmark;
            landmark.setFixedImage(fixedImage);
            landmark.setFixedLandmarks(fixedLandmarks);
            landmark.setMovingLandmarks(movingLandmarks);
            auto ldmTransform = landmark.compute();
            compositeTrans->AddTransform(ldmTransform);
        }

        // Resample intermediate image
        tmpMovingImage =
            ImageTransformResampler(fixedImage, movingImage, compositeTrans);
    } else {
        // Copy the moving image if we didn't do landmark
        tmpMovingImage = movingImage;
    }

    ///// Deformable Registration /////
    auto iterations = parsed["deformable-iterations"].as<int>();
    if (iterations > 0) {
        printf("Running deformable registration...\n");
        rt::DeformableRegistration deformable;
        deformable.setFixedImage(fixedImage);
        deformable.setMovingImage(tmpMovingImage);
        deformable.setNumberOfIterations(iterations);
        auto deformTransform = deformable.compute();

        compositeTrans->AddTransform(deformTransform);
    }

    ///// Write the output image /////
    printf("Writing output image to file...\n");
    auto finalImage =
        ImageTransformResampler(fixedImage, movingImage, compositeTrans);
    auto writer = ImageWriter::New();
    writer->SetInput(finalImage);
    writer->SetFileName(outputImageFileName.string());
    writer->Update();

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
