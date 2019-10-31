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
#include "rt/io/LandmarkReader.hpp"
#include "rt/io/OBJReader.hpp"
#include "rt/io/OBJWriter.hpp"
#include "rt/types/Exceptions.hpp"
#include "rt/types/UVMap.hpp"
#include "rt/Data.hpp"

using namespace rt;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

// Composite Transform
using CompositeTransform = itk::CompositeTransform<double, 2>;

// IO
using OCVBridge = itk::OpenCVImageBridge;
using TransformWriter = itk::TransformFileWriterTemplate<double>;

int main(int argc, char* argv[])
{
    ///// Parse the command line options /////
    // clang-format off
    po::options_description required("General Options");
    required.add_options()
            ("help,h", "Show this message")
            ("moving,m", po::value<std::string>()->required(), "Moving image")
            ("fixed,f", po::value<std::string>()->required(), "OBJ file textured with fixed image")
            ("output-file,o", po::value<std::string>()->required(),
             "Output file path for the retextured OBJ")
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
    printf("Loading files...\n");
    Image8UC3::Pointer fixedImage;
    Image8UC3::Pointer movingImage;

    // Read the OBJ file and static image
    io::OBJReader reader;
    reader.setPath(fixedPath);
    ITKMesh::Pointer origMesh;
    cv::Mat cvFixedImage;
    try {
        origMesh = reader.read();
        cvFixedImage = reader.getTextureMat();
        fixedImage = OCVBridge::CVMatToITKImage<Image8UC3>(cvFixedImage);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Read the moving image
    /*
    auto cvMovingImage = cv::imread(movingPath.string());
    movingImage = OCVBridge::CVMatToITKImage<Image8UC3>(cvMovingImage);
     */
    auto cvMoving = Data::Data::Load(movingPath.string());
    cv::Mat cvMovingImage = cvMoving->getImage();
    movingImage = OCVBridge::CVMatToITKImage<Image8UC3>(cvMovingImage);


    // Ignore spacing information
    fixedImage->SetSpacing(1.0);
    movingImage->SetSpacing(1.0);

    // Setup final transform
    auto compositeTrans = CompositeTransform::New();

    ///// Landmark Registration /////
    if (parsed.count("landmarks")) {
        printf("Running affine registration...\n");
        fs::path landmarksPath = parsed["landmarks"].as<std::string>();
        LandmarkReader landmarkReader(landmarksPath);
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

    ///// Apply the transformation to the UV map /////
    printf("Finished registration\n\n");
    auto oldUVMap = reader.getUVMap();
    UVMap newUVMap;
    for (auto point = origMesh->GetPoints()->Begin();
         point != origMesh->GetPoints()->End(); ++point) {

        cv::Vec2d origUV;
        try {
            origUV = oldUVMap.getUV(point->Index());
        } catch (const std::exception& e) {
            continue;
        }

        // Transform through the final transformation
        auto in = origUV.mul({cvFixedImage.cols, cvFixedImage.rows});
        auto out = compositeTrans->TransformPoint(in.val);
        cv::Vec2d newUV{out[0] / (cvMovingImage.cols - 1),
                        out[1] / (cvMovingImage.rows - 1)};

        // Reassign to UV map
        newUVMap.addUV(newUV);
    }

    ///// Write output mesh /////
    printf("Writing OBJ file...\n");
    io::OBJWriter writer;
    writer.setPath(outputPath);
    writer.setMesh(origMesh);
    writer.setUVMap(newUVMap);
    writer.setTexture(cvMovingImage);
    writer.write();

    ///// Write the final transformations /////
    if (parsed.count("output-tfm")) {
        fs::path transformPath = parsed["output-tfm"].as<std::string>();
        printf("Writing transformation to file...\n");

        // Write deformable transform
        auto transformWriter = TransformWriter::New();
        transformWriter->SetFileName(transformPath.string());
        transformWriter->SetInput(compositeTrans);
        transformWriter->Update();
    }

    return EXIT_SUCCESS;
}
