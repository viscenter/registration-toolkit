#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <smgl/Graph.hpp>

#include "rt/AffineLandmarkRegistration.hpp"
#include "rt/BSplineLandmarkWarping.hpp"
#include "rt/DeformableRegistration.hpp"
#include "rt/ImageTransformResampler.hpp"
#include "rt/LandmarkDetector.hpp"
#include "rt/io/ImageIO.hpp"
#include "rt/io/LandmarkReader.hpp"
#include "rt/io/LandmarkWriter.hpp"
#include "rt/types/CompositeTransform.hpp"

using namespace rt;
using namespace rt::graph;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

// IO

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
        ("output-ldm", po::value<std::string>(),
            "Output file path for the generated landmarks file")
        ("output-tfm,t", po::value<std::string>(),
            "Output file path for the generated transform file")
        ("enable-alpha", "If enabled, an alpha layer will be "
            "added to the moving image if it does not already have one.");

    po::options_description ldmOptions("Landmark Registration Options");
    ldmOptions.add_options()
        ("disable-landmark", "Disable all landmark registration steps")
        ("disable-landmark-bspline", "Disable secondary B-Spline landmark registration")
        ("input-landmarks,l", po::value<std::string>(),
            "Input landmarks file. If not provided, landmark features "
            "are automatically detected from the input images.");

    po::options_description deformOptions("Deformable Registration Options");
    deformOptions.add_options()
        ("disable-deformable", "Disable all deformable registration steps")
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

    ///// Start render graph /////
    smgl::Graph graph;

    ///// Setup input files /////
    auto fixed = std::make_shared<ImageReadNode>();
    fixed->path(fixedPath);
    auto moving = std::make_shared<ImageReadNode>();
    moving->path(movingPath);
    auto compositeTfms = std::make_shared<CompositeTransformNode>();
    graph.insertNodes(fixed, moving, compositeTfms);

    ///// Landmark Registration /////
    auto landmarkTfms = std::make_shared<CompositeTransformNode>();
    graph.insertNode(landmarkTfms);
    if (parsed.count("disable-landmark") == 0) {
        smgl::Node::Pointer ldmNode;
        // Load landmarks from file
        if (parsed.count("input-landmarks") > 0) {
            auto readLdm = std::make_shared<LandmarkReaderNode>();
            readLdm->path(parsed["input-landmarks"].as<std::string>());
            ldmNode = readLdm;
        }
        // Generate landmarks automatically
        else {
            auto genLdm = std::make_shared<LandmarkDetectorNode>();
            fixed->image >> genLdm->fixedImage;
            moving->image >> genLdm->movingImage;
            ldmNode = genLdm;

            // Optionally write generated landmarks to file
            if (parsed.count("output-ldm") > 0) {
                auto writer = std::make_shared<LandmarkWriterNode>();
                writer->path(parsed["output-ldm"].as<std::string>());
                genLdm->fixedLandmarks >> writer->fixed;
                genLdm->movingLandmarks >> writer->moving;
                graph.insertNode(writer);
            }
        }
        graph.insertNode(ldmNode);

        // Run affine registration
        auto affine = std::make_shared<AffineLandmarkRegistrationNode>();
        ldmNode->getOutputPort("fixedLandmarks") >> affine->fixedLandmarks;
        ldmNode->getOutputPort("movingLandmarks") >> affine->movingLandmarks;
        graph.insertNode(affine);

        // Transform
        affine->transform >> landmarkTfms->lhs;

        // B-Spline landmark warping
        if (parsed.count("disable-landmark-bspline") == 0) {
            // Update the landmark positions
            auto tfmLdm = std::make_shared<TransformLandmarksNode>();
            affine->transform >> tfmLdm->transform;
            ldmNode->getOutputPort("movingLandmarks") >> tfmLdm->landmarksIn;
            graph.insertNode(tfmLdm);

            // BSpline Warp
            auto bspline = std::make_shared<BSplineLandmarkWarpingNode>();
            fixed->image >> bspline->fixedImage;
            ldmNode->getOutputPort("fixedLandmarks") >> bspline->fixedLandmarks;
            tfmLdm->landmarksOut >> bspline->movingLandmarks;
            bspline->transform >> landmarkTfms->rhs;
            graph.insertNode(bspline);
        }

        // Add landmark transforms to final transforms
        landmarkTfms->result >> compositeTfms->lhs;
    }

    ///// Deformable Registration /////
    if (parsed.count("disable-deformable") == 0) {
        // Resample moving image for next stage
        auto resample1 = std::make_shared<ImageResampleNode>();
        fixed->image >> resample1->fixedImage;
        moving->image >> resample1->movingImage;
        landmarkTfms->result >> resample1->transform;
        graph.insertNode(resample1);

        // Compute deformable
        auto deformable = std::make_shared<DeformableRegistrationNode>();
        deformable->iterations(parsed["deformable-iterations"].as<int>());
        fixed->image >> deformable->fixedImage;
        resample1->resampledImage >> deformable->movingImage;
        graph.insertNode(deformable);

        // Add transform to final composite
        deformable->transform >> compositeTfms->rhs;
    }

    ///// Resample the source image /////
    auto resample2 = std::make_shared<ImageResampleNode>();
    fixed->image >> resample2->fixedImage;
    moving->image >> resample2->movingImage;
    compositeTfms->result >> resample2->transform;
    graph.insertNode(resample2);

    ///// Write the output image /////
    auto writer = std::make_shared<ImageWriteNode>();
    writer->path(outputPath);
    resample2->resampledImage >> writer->image;
    graph.insertNode(writer);

    ///// Write the final transformations /////
    if (parsed.count("output-tfm") > 0) {
        auto tfmWriter = std::make_shared<WriteTransformNode>();
        tfmWriter->path(parsed["output-tfm"].as<std::string>());
        compositeTfms->result >> tfmWriter->transform;
        graph.insertNode(tfmWriter);
    }

    // Compute result
    graph.update();

    return EXIT_SUCCESS;
}
