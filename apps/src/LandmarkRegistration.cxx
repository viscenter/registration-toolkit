#include <fstream>
#include <iostream>

#include <itkCompositeTransform.h>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageRegistrationMethod.h>
#include <itkLandmarkBasedTransformInitializer.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>
#include <itkTransformFileWriter.h>

#include "rt/DeformableRegistration.hpp"
#include "rt/ImageTypes.hpp"

// Defines
constexpr static uint8_t EmptyPixel = 0;

// Image Resampling and Filtering
using ResampleFilter = itk::ResampleImageFilter<Image8UC3, Image8UC3, double>;
using ColorInterpolator =
    itk::NearestNeighborInterpolateImageFunction<Image8UC3, double>;

// Landmark Transform Types
using AffineTransform = itk::AffineTransform<double, 2>;
using LandmarkTransformInitializer =
    itk::LandmarkBasedTransformInitializer<AffineTransform, Image8UC3, Image8UC3>;
using LandmarkContainer = LandmarkTransformInitializer::LandmarkPointContainer;
using Landmark = LandmarkTransformInitializer::LandmarkPointType;

// Composite Transform
using CompositeTransform = itk::CompositeTransform<double, 2>;

// IO
using ImageReader = itk::ImageFileReader<Image8UC3>;
using ImageWriter = itk::ImageFileWriter<Image8UC3>;
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
    ImageReader::Pointer imgReader;
    Image8UC3::Pointer fixedImage;
    Image8UC3::Pointer movingImage;

    // Read the two images
    try {
        // Read the fixed image
        imgReader = ImageReader::New();
        imgReader->SetFileName(fixedImageFileName);
        imgReader->Update();
        fixedImage = imgReader->GetOutput();

        // Read the moving image
        imgReader = ImageReader::New();
        imgReader->SetFileName(movingImageFileName);
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

    // Read the landmarks file
    LandmarkContainer fixedLandmarks, movingLandmarks;
    Landmark fixedPoint, movingPoint;
    Image8UC3::IndexType fixedIndex, movingIndex;
    size_t fixedX, fixedY, movingX, movingY;

    std::ifstream pointsFile(landmarksFileName);
    while (!pointsFile.fail()) {
        pointsFile >> fixedX >> fixedY >> movingX >> movingY;

        fixedIndex[0] = fixedX;
        fixedIndex[1] = fixedY;
        movingIndex[0] = movingX;
        movingIndex[1] = movingY;

        // Transform landmarks in case spacing still gets used
        fixedImage->TransformIndexToPhysicalPoint(fixedIndex, fixedPoint);
        movingImage->TransformIndexToPhysicalPoint(movingIndex, movingPoint);

        fixedLandmarks.push_back(fixedPoint);
        movingLandmarks.push_back(movingPoint);
    }
    pointsFile.close();

    ///// Landmark Registration /////
    printf("Beginning landmark warping\n");

    // Generate the affine deformTransform
    LandmarkTransformInitializer::Pointer ldmTransformInit =
        LandmarkTransformInitializer::New();
    ldmTransformInit->SetFixedLandmarks(fixedLandmarks);
    ldmTransformInit->SetMovingLandmarks(movingLandmarks);

    AffineTransform::Pointer ldmTransform = AffineTransform::New();
    ldmTransform->SetIdentity();
    ldmTransformInit->SetTransform(ldmTransform);
    ldmTransformInit->InitializeTransform();

    // Apply it to the image
    ResampleFilter::Pointer resample = ResampleFilter::New();
    ColorInterpolator::Pointer interpolator = ColorInterpolator::New();
    resample->SetInput(movingImage);
    resample->SetTransform(ldmTransform);
    resample->SetInterpolator(interpolator);
    resample->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
    resample->SetOutputOrigin(fixedImage->GetOrigin());
    resample->SetOutputSpacing(fixedImage->GetSpacing());
    resample->SetOutputDirection(fixedImage->GetDirection());
    resample->SetDefaultPixelValue(EmptyPixel);
    resample->GetOutput();

    // Write the image
    ImageWriter::Pointer writer = ImageWriter::New();
    writer->SetInput(resample->GetOutput());
    writer->SetFileName(outputImageFileName);
    writer->Update();

    printf("Finished landmark warping\n");

    ///// Deformable Registration /////
    rt::DeformableRegistration deformable;
    deformable.setFixedImage(fixedImage);
    deformable.setMovingImage(resample->GetOutput());
    auto deformTransform = deformable.compute();

    auto compositeTrans = CompositeTransform::New();
    compositeTrans->AddTransform(ldmTransform);
    compositeTrans->AddTransform(deformTransform);

    ResampleFilter::Pointer resample2 = ResampleFilter::New();
    resample2->SetInput(movingImage);
    resample2->SetTransform(compositeTrans);
    resample2->SetInterpolator(interpolator);
    resample2->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
    resample2->SetOutputOrigin(fixedImage->GetOrigin());
    resample2->SetOutputSpacing(fixedImage->GetSpacing());
    resample2->SetOutputDirection(fixedImage->GetDirection());
    resample2->SetDefaultPixelValue(EmptyPixel);
    resample2->GetOutput();

    // Write the image
    writer = ImageWriter::New();
    writer->SetInput(resample2->GetOutput());
    writer->SetFileName(outputImageFileName);
    writer->Update();

    printf("Finished registration\n\n");

    ///// Write the final transformations /////
    printf("Writing transformation to file\n");

    // Write deformable transform
    TransformWriter::Pointer transformWriter = TransformWriter::New();
    transformWriter->SetFileName(transformFileName);
    transformWriter->SetInput(compositeTrans);
    transformWriter->Update();

    printf("Finished writing transformation to file\n\n");
    //
    //    printf("Time and memory usage information:\n");
    //    chronometer.Stop("LandmarkRegistration");
    //    memorymeter.Stop("LandmarkRegistration");
    //    chronometer.Report(std::cout);
    //    memorymeter.Report(std::cout);

    return EXIT_SUCCESS;
}
