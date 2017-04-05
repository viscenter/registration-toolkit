#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>
#include <itkCompositeTransform.h>
#include <itkTransformFileWriter.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vc/core/io/OBJReader.hpp>
#include <vc/core/types/Exceptions.hpp>
#include <vc/core/types/UVMap.hpp>

#include "rt/DeformableRegistration.hpp"
#include "rt/ImageTransformResampler.hpp"
#include "rt/ImageTypes.hpp"
#include "rt/LandmarkIO.hpp"
#include "rt/LandmarkRegistration.hpp"
#include "rt/itk/itkOpenCVImageBridge.h"

using namespace rt;
namespace fs = boost::filesystem;

// Composite Transform
using CompositeTransform = itk::CompositeTransform<double, 2>;

// IO
using TransformWriter = itk::TransformFileWriterTemplate<double>;

int main(int argc, char* argv[])
{
    if (argc < 7) {
        std::cerr << "Missing Parameters " << std::endl;
        std::cerr << "Usage: " << argv[0];
        std::cerr << "objFile landmarksFile ";
        std::cerr << "movingImage ";
        std::cerr << "transformDestination ";
        std::cerr << "numberOfIterations " << std::endl;
        return EXIT_FAILURE;
    }

    auto landmarksFileName = argv[1];
    auto objFileName = argv[2];
    auto movingImageFileName = argv[3];
    auto transformFileName = argv[4];
    auto iterationsIn = argv[5];

    printf("%-17s\n\n", "Registration Parameters");
    printf("%-17s %s\n", "Obj file: ", objFileName);
    printf("%-17s %s\n", "Landmarks file: ", landmarksFileName);
    printf("%-17s %s\n", "Moving image: ", movingImageFileName);
    printf("%-17s %s\n", "Iterations: ", iterationsIn);

    ///// Setup input files /////
    Image8UC3::Pointer fixedImage;
    Image8UC3::Pointer movingImage;

    // Read the OBJ file
    volcart::io::OBJReader reader;
    reader.setPath(objFileName);
    try {
        reader.read();
        fixedImage = itk::OpenCVImageBridge::CVMatToITKImage<Image8UC3>(
            reader.getTextureMat());
    } catch (volcart::IOException& excp) {
        std::cerr << "Exception thrown " << std::endl;
        std::cerr << excp.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Read the two images
    movingImage = itk::OpenCVImageBridge::CVMatToITKImage<Image8UC3>(
        cv::imread(movingImageFileName));

    // Ignore spacing information
    fixedImage->SetSpacing(1.0);
    movingImage->SetSpacing(1.0);

    // Read the landmarks file
    LandmarkIO landmarkReader(landmarksFileName);
    landmarkReader.setFixedImage(fixedImage);
    landmarkReader.setMovingImage(movingImage);
    landmarkReader.read();
    auto fixedLandmarks = landmarkReader.getFixedLandmarks();
    auto movingLandmarks = landmarkReader.getMovingLandmarks();

    ///// Landmark Registration /////
    printf("Running landmark registration...\n");

    // Generate the landmark transform
    LandmarkRegistration landmark;
    landmark.setFixedLandmarks(fixedLandmarks);
    landmark.setMovingLandmarks(movingLandmarks);
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
