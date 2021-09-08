#include <iostream>

#include <boost/program_options.hpp>
#include <smgl/Graph.hpp>
#include <smgl/Graphviz.hpp>

#include "rt/filesystem.hpp"
#include "rt/graph.hpp"

using namespace rt;
using namespace rt::graph;

namespace po = boost::program_options;
namespace fs = rt::filesystem;

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

    po::options_description graphOptions("Render Graph Options");
    graphOptions.add_options()
        ("output-graph,g", po::value<std::string>(), "Render graph JSON file")
        ("output-dot", po::value<std::string>(), "Render graph Dot file");

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
    all.add(required).add(graphOptions).add(ldmOptions).add(deformOptions);
    // clang-format on

    // Parse the cmd line
    po::variables_map parsed;
    po::store(po::command_line_parser(argc, argv).options(all).run(), parsed);

    // Show the help message
    if (parsed.count("help") > 0 or argc < 2) {
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
    rt::graph::RegisterNodes();
    smgl::Graph graph;

    ///// Setup caching /////
    if (parsed.count("output-graph") > 0) {
        fs::path cacheFile = parsed["output-graph"].as<std::string>();
        graph.setEnableCache(true);
        graph.setCacheFile(cacheFile);
    }

    ///// Setup input files /////
    auto fixed = graph.insertNode<ImageReadNode>();
    fixed->path = fixedPath;
    auto moving = graph.insertNode<ImageReadNode>();
    moving->path = movingPath;
    auto compositeTfms = graph.insertNode<CompositeTransformNode>();

    ///// Landmark Registration /////
    auto landmarkTfms = graph.insertNode<CompositeTransformNode>();
    if (parsed.count("disable-landmark") == 0) {
        smgl::Node::Pointer ldmNode;
        // Load landmarks from file
        if (parsed.count("input-landmarks") > 0) {
            auto readLdm = graph.insertNode<LandmarkReaderNode>();
            readLdm->path = parsed["input-landmarks"].as<std::string>();
            ldmNode = readLdm;
        }
        // Generate landmarks automatically
        else {
            auto genLdm = graph.insertNode<LandmarkDetectorNode>();
            genLdm->fixedImage = fixed->image;
            genLdm->movingImage = moving->image;
            ldmNode = genLdm;

            // Optionally write generated landmarks to file
            if (parsed.count("output-ldm") > 0) {
                auto writer = graph.insertNode<LandmarkWriterNode>();
                writer->path = parsed["output-ldm"].as<std::string>();
                writer->fixed = genLdm->fixedLandmarks;
                writer->moving = genLdm->movingLandmarks;
            }
        }

        // Run affine registration
        auto affine = graph.insertNode<AffineLandmarkRegistrationNode>();
        affine->fixedLandmarks = ldmNode->getOutputPort("fixedLandmarks");
        affine->movingLandmarks = ldmNode->getOutputPort("movingLandmarks");

        // Transform
        landmarkTfms->first = affine->transform;

        // B-Spline landmark warping
        if (parsed.count("disable-landmark-bspline") == 0) {
            // Update the landmark positions
            auto tfmLdm = graph.insertNode<TransformLandmarksNode>();
            tfmLdm->transform = affine->transform;
            tfmLdm->landmarksIn = ldmNode->getOutputPort("movingLandmarks");

            // BSpline Warp
            auto bspline = graph.insertNode<BSplineLandmarkWarpingNode>();
            bspline->fixedImage = fixed->image;
            bspline->fixedLandmarks = ldmNode->getOutputPort("fixedLandmarks");
            bspline->movingLandmarks = tfmLdm->landmarksOut;
            landmarkTfms->second = bspline->transform;
        }

        // Add landmark transforms to final transforms
        compositeTfms->first = landmarkTfms->result;
    }

    ///// Deformable Registration /////
    if (parsed.count("disable-deformable") == 0) {
        // Resample moving image for next stage
        auto resample1 = graph.insertNode<ImageResampleNode>();
        resample1->fixedImage = fixed->image;
        resample1->movingImage = moving->image;
        resample1->transform = landmarkTfms->result;

        // Compute deformable
        auto deformable = graph.insertNode<DeformableRegistrationNode>();
        deformable->iterations = parsed["deformable-iterations"].as<int>();
        deformable->fixedImage = fixed->image;
        deformable->movingImage = resample1->resampledImage;

        // Add transform to final composite
        compositeTfms->second = deformable->transform;
    }

    ///// Resample the source image /////
    auto resample2 = graph.insertNode<ImageResampleNode>();
    resample2->fixedImage = fixed->image;
    resample2->movingImage = moving->image;
    resample2->transform = compositeTfms->result;

    ///// Write the output image /////
    auto writer = graph.insertNode<ImageWriteNode>();
    writer->path = outputPath;
    writer->image = resample2->resampledImage;

    ///// Write the final transformations /////
    if (parsed.count("output-tfm") > 0) {
        auto tfmWriter = graph.insertNode<WriteTransformNode>();
        tfmWriter->path = parsed["output-tfm"].as<std::string>();
        tfmWriter->transform = compositeTfms->result;
    }

    // Compute result
    graph.update();

    // Write Dot file
    if (parsed.count("output-dot") > 0) {
        smgl::WriteDotFile(parsed["output-dot"].as<std::string>(), graph);
    }

    return EXIT_SUCCESS;
}
