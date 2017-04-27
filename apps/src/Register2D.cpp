#include <fstream>
#include <iostream>

#include <itkCompositeTransform.h>
#include <itkTransformFileWriter.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "rt/DeformableRegistration.hpp"
#include "rt/ImageTransformResampler.hpp"
#include "rt/ImageTypes.hpp"
#include "rt/LandmarkDetector.hpp"
#include "rt/LandmarkIO.hpp"
#include "rt/LandmarkRegistration.hpp"
#include "rt/itk/itkOpenCVImageBridge.h"

static constexpr int NUM_BEST_MATCHES = 30;

using namespace rt;

// Composite Transform
using CompositeTransform = itk::CompositeTransform<double, 2>;

// IO
using TransformWriter = itk::TransformFileWriterTemplate<double>;

int main(int argc, char* argv[])
{
    if (argc < 7) {
        std::cerr << "Missing Parameters " << std::endl;
        std::cerr << "Usage: " << argv[0];
        std::cerr << " landmarksFile fixedImage ";
        std::cerr << "movingImage outputImageFile ";
        std::cerr << "transformDestination ";
        std::cerr << "numberOfIterations " << std::endl;
        return EXIT_FAILURE;
    }

    auto landmarksFileName = argv[1];
    auto fixedImageFileName = argv[2];
    auto movingImageFileName = argv[3];
    auto outputImageFileName = argv[4];
    auto transformFileName = argv[5];
    auto iterationsIn = argv[6];

    printf("%-17s\n\n", "Registration Parameters");
    printf("%-17s %s\n", "Landmarks file: ", landmarksFileName);
    printf("%-17s %s\n", "Fixed image: ", fixedImageFileName);
    printf("%-17s %s\n", "Moving image: ", movingImageFileName);
    printf("%-17s %s\n", "Output image: ", outputImageFileName);
    printf("%-17s %s\n", "Transform file:", transformFileName);
    printf("%-17s %s\n", "Iterations: ", iterationsIn);

    ///// Setup input files /////
    Image8UC3::Pointer fixedImage;
    Image8UC3::Pointer movingImage;
    cv::Mat cvFixedImage, cvMovingImage;

    // Read the two images
    try {
        // Read the fixed image
        cvFixedImage = cv::imread(fixedImageFileName);
        fixedImage = itk::OpenCVImageBridge::CVMatToITKImage<rt::Image8UC3>(
            cvFixedImage);

        // Read the moving image
        cvMovingImage = cv::imread(movingImageFileName);
        movingImage = itk::OpenCVImageBridge::CVMatToITKImage<rt::Image8UC3>(
            cvMovingImage);
    } catch (itk::ExceptionObject& excp) {
        std::cerr << "Exception thrown " << std::endl;
        std::cerr << excp << std::endl;
        return EXIT_FAILURE;
    }

    // Ignore spacing information
    fixedImage->SetSpacing(1.0);
    movingImage->SetSpacing(1.0);

    // Read the landmarks file
    LandmarkDetector detector;
    detector.setFixedImage(cvFixedImage);
    detector.setMovingImage(cvMovingImage);

    auto matchedpairs = detector.compute(NUM_BEST_MATCHES);

    LandmarkRegistration::LandmarkContainer fixedPts, movingPts;
    // Write the data
    for (auto& p : matchedpairs) {
        // Fixed
        LandmarkRegistration::Landmark fixed, moving;
        fixed[0] = p.first.x;
        fixed[1] = p.first.y;
        moving[0] = p.second.x;
        moving[1] = p.second.y;
        fixedPts.push_back(fixed);
        //Moving
        movingPts.push_back(moving);
    }


    ///// Landmark Registration /////
    printf("Running landmark registration...\n");

    // Generate the landmark transform
    LandmarkRegistration landmark;
    landmark.setFixedLandmarks(fixedPts);
    landmark.setMovingLandmarks(movingPts);
    auto ldmTransform = landmark.compute();

    // Apply it to the image
    auto tmpMovingImage =
        ImageTransformResampler(fixedImage, movingImage, ldmTransform);

    ///// Deformable Registration /////
    printf("Running deformable registration...\n");
    rt::DeformableRegistration deformable;
    deformable.setFixedImage(fixedImage);
    deformable.setMovingImage(tmpMovingImage);
    auto deformTransform = deformable.compute();

    ///// Combine transforms /////
    auto compositeTrans = CompositeTransform::New();
    compositeTrans->AddTransform(ldmTransform);
    compositeTrans->AddTransform(deformTransform);

    ///// Write the output image /////
    printf("Writing output image to file...\n");
    auto finalImage =
        ImageTransformResampler(fixedImage, movingImage, compositeTrans);
    auto cvFinalImage = itk::OpenCVImageBridge::ITKImageToCVMat<rt::Image8UC3>(finalImage);
    cv::imwrite(outputImageFileName, cvFinalImage);

    printf("Finished registration\n\n");

    ///// Write the final transformations /////
    printf("Writing transformation to file...\n");

    // Write deformable transform
    TransformWriter::Pointer transformWriter = TransformWriter::New();
    transformWriter->SetFileName(transformFileName);
    transformWriter->SetInput(compositeTrans);
    transformWriter->Update();

    printf("Registration complete!\n\n");

    return EXIT_SUCCESS;
}
