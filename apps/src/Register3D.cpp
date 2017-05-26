#include <iostream>

#include <boost/filesystem.hpp>
#include <itkCompositeTransform.h>
#include <itkTransformFileWriter.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <vc/core/io/OBJReader.hpp>
#include <vc/core/io/OBJWriter.hpp>
#include <vc/core/types/Exceptions.hpp>
#include <vc/core/types/UVMap.hpp>

#include "rt/BSplineLandmarkWarping.hpp"
#include "rt/DeformableRegistration.hpp"
#include "rt/ImageTransformResampler.hpp"
#include "rt/ImageTypes.hpp"
#include "rt/LandmarkReader.hpp"
#include "rt/itk/itkOpenCVImageBridge.h"

namespace fs = boost::filesystem;
namespace vc = volcart;

// Composite Transform
using CompositeTransform = itk::CompositeTransform<double, 2>;

// IO
using TransformWriter = itk::TransformFileWriterTemplate<double>;

int main(int argc, char* argv[])
{
    if (argc < 7) {
        std::cerr << "Missing Parameters " << std::endl;
        std::cerr << "Usage: " << argv[0];
        std::cerr << " objPath landmarksPath ";
        std::cerr << "movingImagePath iterations ";
        std::cerr << "objOutPath  ";
        std::cerr << "transformOutPath " << std::endl;
        return EXIT_FAILURE;
    }

    fs::path objPath(argv[1]);
    fs::path landmarksPath(argv[2]);
    fs::path movingImagePath(argv[3]);
    size_t iterations = std::stoull(argv[4]);
    fs::path outPath(argv[5]);
    fs::path transformOutPath(argv[6]);

    printf("%-17s\n\n", "Registration Parameters");
    printf("%-17s %s\n", "Obj path: ", objPath.c_str());
    printf("%-17s %s\n", "Landmarks path: ", landmarksPath.c_str());
    printf("%-17s %s\n", "Moving image path: ", movingImagePath.c_str());
    printf("%-17s %zu\n", "Iterations: ", iterations);

    ///// Setup input files /////
    rt::Image8UC3::Pointer fixedImage;
    rt::Image8UC3::Pointer movingImage;

    // Read the OBJ file and static image
    vc::io::OBJReader reader;
    reader.setPath(objPath);
    vc::ITKMesh::Pointer origMesh;
    cv::Mat cvFixedImage;
    try {
        origMesh = reader.read();
        cvFixedImage = reader.getTextureMat();
        fixedImage = itk::OpenCVImageBridge::CVMatToITKImage<rt::Image8UC3>(
            cvFixedImage);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Read the moving image
    auto cvMovingImage = cv::imread(movingImagePath.string());
    movingImage =
        itk::OpenCVImageBridge::CVMatToITKImage<rt::Image8UC3>(cvMovingImage);

    // Ignore spacing information
    fixedImage->SetSpacing(1.0);
    movingImage->SetSpacing(1.0);

    // Read the landmarks file
    rt::LandmarkReader landmarkReader(landmarksPath);
    landmarkReader.setFixedImage(fixedImage);
    landmarkReader.setMovingImage(movingImage);
    landmarkReader.read();
    auto fixedLandmarks = landmarkReader.getFixedLandmarks();
    auto movingLandmarks = landmarkReader.getMovingLandmarks();

    ///// Landmark Registration /////
    printf("Running landmark registration...\n");

    // Generate the landmark transform
    rt::BSplineLandmarkWarping landmark;
    landmark.setFixedImage(fixedImage);
    landmark.setFixedLandmarks(fixedLandmarks);
    landmark.setMovingLandmarks(movingLandmarks);
    auto ldmTransform = landmark.compute();

    // Apply it to the image
    auto tmpMovingImage =
        rt::ImageTransformResampler(fixedImage, movingImage, ldmTransform);

    ///// Deformable Registration /////
    printf("Running deformable registration...\n");
    rt::DeformableRegistration deformable;
    deformable.setFixedImage(fixedImage);
    deformable.setMovingImage(tmpMovingImage);
    deformable.setNumberOfIterations(iterations);
    auto deformTransform = deformable.compute();

    ///// Combine transforms /////
    auto compositeTrans = CompositeTransform::New();
    compositeTrans->AddTransform(ldmTransform);
    compositeTrans->AddTransform(deformTransform);

    ///// Apply the transformation to the UV map /////
    printf("Finished registration\n\n");
    auto uvMap = reader.getUVMap();
    for (auto point = origMesh->GetPoints()->Begin();
         point != origMesh->GetPoints()->End(); ++point) {

        // Get the UV mapping
        auto origUV = uvMap.get(point->Index());
        if (origUV == vc::NULL_MAPPING) {
            continue;
        }

        // Transform through the final transformation
        auto in = origUV.mul({cvFixedImage.cols, cvFixedImage.rows});
        auto out = compositeTrans->TransformPoint(in.val);
        cv::Vec2d newUV{out[0] / (cvMovingImage.cols - 1),
                        out[1] / (cvMovingImage.rows - 1)};

        // Reassign to UV map
        uvMap.set(point->Index(), newUV);
    }

    ///// Write output mesh /////
    printf("Writing OBJ file...\n");
    vc::io::OBJWriter writer;
    writer.setPath(outPath);
    writer.setMesh(origMesh);
    writer.setUVMap(uvMap);
    writer.setTexture(cvMovingImage);
    writer.write();

    ///// Write the final transformations /////
    printf("Writing transformation to file...\n");
    auto transformWriter = TransformWriter::New();
    transformWriter->SetFileName(transformOutPath.string());
    transformWriter->SetInput(compositeTrans);
    transformWriter->Update();

    printf("Registration complete!\n\n");

    return EXIT_SUCCESS;
}
