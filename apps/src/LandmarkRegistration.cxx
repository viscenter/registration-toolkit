#include <fstream>
#include <iostream>

#include <itkCompositeTransform.h>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageRegistrationMethod.h>
#include <itkLandmarkBasedTransformInitializer.h>
#include <itkMattesMutualInformationImageToImageMetric.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkRGBToLuminanceImageFilter.h>
#include <itkRegularStepGradientDescentOptimizer.h>
#include <itkResampleImageFilter.h>
#include <itkTransformFileWriter.h>

// Base types
constexpr static uint8_t EmptyPixel = 0;
using ColorPixel = itk::RGBPixel<uint8_t>;
using Image = itk::Image<ColorPixel, 2>;
using Vector = itk::Vector<double, 2>;
using DeformationField = itk::Image<Vector, 2>;
using GrayPixel = unsigned char;
using GrayImage = itk::Image<GrayPixel, 2>;

// Image Resampling and Filtering
using ResampleFilter = itk::ResampleImageFilter<Image, Image, double>;
using ColorInterpolator =
    itk::NearestNeighborInterpolateImageFunction<Image, double>;
using GrayInterpolator =
    itk::NearestNeighborInterpolateImageFunction<GrayImage, double>;
using GrayscaleFilter = itk::RGBToLuminanceImageFilter<Image, GrayImage>;

// Landmark Transform Types
using AffineTransform = itk::AffineTransform<double, 2>;
using LandmarkTransformInitializer =
    itk::LandmarkBasedTransformInitializer<AffineTransform, Image, Image>;
using LandmarkContainer = LandmarkTransformInitializer::LandmarkPointContainer;
using Landmark = LandmarkTransformInitializer::LandmarkPointType;

// Deformable Registration Types
using Metric =
    itk::MattesMutualInformationImageToImageMetric<GrayImage, GrayImage>;
using Optimizer = itk::RegularStepGradientDescentOptimizer;
using Registration = itk::ImageRegistrationMethod<GrayImage, GrayImage>;
using BSplineTransform = itk::BSplineTransform<double, 2, 3>;
using BSplineParameters = BSplineTransform::ParametersType;

// Composite Transform
using CompositeTransform = itk::CompositeTransform<double, 2>;

// IO
using ImageReader = itk::ImageFileReader<Image>;
using ImageWriter = itk::ImageFileWriter<Image>;
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
    Image::Pointer fixedImage;
    Image::Pointer movingImage;

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
    Image::IndexType fixedIndex, movingIndex;
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
    // Create grayscale images
    GrayscaleFilter::Pointer fixedFilter = GrayscaleFilter::New();
    fixedFilter->SetInput(fixedImage);

    GrayscaleFilter::Pointer movingFilter = GrayscaleFilter::New();
    movingFilter->SetInput(resample->GetOutput());

    // Setup
    Metric::Pointer metric = Metric::New();
    Optimizer::Pointer optimizer = Optimizer::New();
    Registration::Pointer registration = Registration::New();
    BSplineTransform::Pointer deformTransform = BSplineTransform::New();
    GrayInterpolator::Pointer grayInterpolator = GrayInterpolator::New();

    registration->SetMetric(metric);
    registration->SetOptimizer(optimizer);
    registration->SetInterpolator(grayInterpolator);
    registration->SetTransform(deformTransform);
    registration->SetFixedImage(fixedFilter->GetOutput());
    registration->SetMovingImage(movingFilter->GetOutput());

    Image::RegionType fixedRegion = fixedImage->GetBufferedRegion();
    registration->SetFixedImageRegion(fixedRegion);

    ///// Deformable parameters /////
    uint16_t transformMeshFillSize = 12;

    // The maximum step length when the optimizer starts moving around
    double maxStepLength =
        fixedImage->GetLargestPossibleRegion().GetSize()[0] / 500.0;
    // Registration will stop if the step length drops below this value
    double minStepLength =
        fixedImage->GetLargestPossibleRegion().GetSize()[0] / 500000.0;

    // Optimizer step length is reduced by this factor each iteration
    double relaxationFactor = 0.85;

    // Hard iteration limit
    int numberOfIterations = atoi(iterationsIn);

    // The registration process will stop if the metric starts changing less
    // than this
    double gradientMagnitudeTolerance = 0.0001;

    /* The metric requires two parameters to be selected: the number
       of bins used to compute the entropy and the number of spatial samples
       used to compute the density estimates. In typical application, 50
       histogram bins are sufficient and the metric is relatively insensitive
       to changes in the number of bins. The number of spatial samples
       to be used depends on the content of the image. If the images are
       smooth and do not contain much detail, then using approximately
       1 percent of the pixels will do. On the other hand, if the images
       are detailed, it may be necessary to use a much higher proportion,
       such as 20 percent. */
    int numberOfHistogramBins = 50;
    auto numberOfSamples =
        static_cast<unsigned int>(fixedRegion.GetNumberOfPixels() / 80.0);

    /////////////////////////////////

    BSplineTransform::PhysicalDimensionsType FixedPhysicalDims;
    BSplineTransform::MeshSizeType MeshSize;
    BSplineTransform::OriginType FixedOrigin;

    for (unsigned int i = 0; i < 2; i++) {
        FixedOrigin[i] = fixedImage->GetOrigin()[i];
        FixedPhysicalDims[i] =
            fixedImage->GetSpacing()[i] *
            static_cast<double>(
                fixedImage->GetLargestPossibleRegion().GetSize()[i] - 1);
    }

    MeshSize.Fill(transformMeshFillSize);

    deformTransform->SetTransformDomainOrigin(FixedOrigin);
    deformTransform->SetTransformDomainPhysicalDimensions(FixedPhysicalDims);
    deformTransform->SetTransformDomainMeshSize(MeshSize);
    deformTransform->SetTransformDomainDirection(fixedImage->GetDirection());

    const auto numberOfParameters = deformTransform->GetNumberOfParameters();
    BSplineParameters parameters(numberOfParameters);
    parameters.Fill(0.0);
    deformTransform->SetParameters(parameters);
    registration->SetInitialTransformParameters(
        deformTransform->GetParameters());

    optimizer->MinimizeOn();
    optimizer->SetMaximumStepLength(maxStepLength);
    optimizer->SetMinimumStepLength(minStepLength);
    optimizer->SetRelaxationFactor(relaxationFactor);
    optimizer->SetNumberOfIterations(numberOfIterations);
    optimizer->SetGradientMagnitudeTolerance(gradientMagnitudeTolerance);

    // Create the Command observer and register it with the optimizer.
    //    CommandIterationUpdate::Pointer observer =
    //    CommandIterationUpdate::New();
    //    optimizer->AddObserver(itk::IterationEvent(), observer);

    metric->SetNumberOfHistogramBins(numberOfHistogramBins);
    metric->SetNumberOfSpatialSamples(numberOfSamples);

    printf("\nStarting registration\n");

    try {
        registration->Update();

        std::cout << "Optimizer stop condition = "
                  << registration->GetOptimizer()->GetStopConditionDescription()
                  << std::endl;
    } catch (itk::ExceptionObject& err) {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    auto finalParams = registration->GetLastTransformParameters();
    deformTransform->SetParameters(finalParams);

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
