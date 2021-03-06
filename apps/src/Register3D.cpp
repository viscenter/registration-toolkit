#include <iostream>

#include <boost/program_options.hpp>
#include <smgl/Graph.hpp>
#include <smgl/Graphviz.hpp>

#include "rt/filesystem.hpp"
#include "rt/graph/Nodes.hpp"

using namespace rt;
using namespace rt::graph;

namespace fs = rt::filesystem;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    ///// Parse the command line options /////
    // clang-format off
    po::options_description required("General Options");
    required.add_options()
        ("help,h", "Show this message")
        ("moving,m", po::value<std::string>()->required(), "Moving image")
        ("fixed,f", po::value<std::string>()->required(),
            "OBJ file textured with fixed image")
        ("output-file,o", po::value<std::string>()->required(),
            "Output file path for the retextured OBJ")
        ("output-tfm,t", po::value<std::string>(),
            "Output file path for the generated transform file");

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
    rt::graph::RegisterAllNodeTypes();
    smgl::Graph graph;

    ///// Setup caching /////
    if (parsed.count("output-graph") > 0) {
        fs::path cacheFile = parsed["output-graph"].as<std::string>();
        graph.setEnableCache(true);
        graph.setCacheFile(cacheFile);
    }

    ///// Setup input files /////
    // Read the OBJ file and static image
    auto fixed = graph.insertNode<MeshReadNode>();
    fixed->path(fixedPath);
    auto moving = graph.insertNode<ImageReadNode>();
    moving->path(movingPath);
    auto compositeTfms = graph.insertNode<CompositeTransformNode>();

    ///// Landmark Registration /////
    auto landmarkTfms = graph.insertNode<CompositeTransformNode>();
    if (parsed.count("disable-landmark") == 0) {
        smgl::Node::Pointer ldmNode;
        // Load landmarks from file
        if (parsed.count("input-landmarks") > 0) {
            auto readLdm = graph.insertNode<LandmarkReaderNode>();
            readLdm->path(parsed["input-landmarks"].as<std::string>());
            ldmNode = readLdm;
        }
        // Generate landmarks automatically
        else {
            auto genLdm = graph.insertNode<LandmarkDetectorNode>();
            fixed->image >> genLdm->fixedImage;
            moving->image >> genLdm->movingImage;
            ldmNode = genLdm;

            // Optionally write generated landmarks to file
            if (parsed.count("output-ldm") > 0) {
                auto writer = graph.insertNode<LandmarkWriterNode>();
                writer->path(parsed["output-ldm"].as<std::string>());
                genLdm->fixedLandmarks >> writer->fixed;
                genLdm->movingLandmarks >> writer->moving;
            }
        }

        // Run affine registration
        auto affine = graph.insertNode<AffineLandmarkRegistrationNode>();
        ldmNode->getOutputPort("fixedLandmarks") >> affine->fixedLandmarks;
        ldmNode->getOutputPort("movingLandmarks") >> affine->movingLandmarks;

        // Transform
        affine->transform >> landmarkTfms->first;

        // B-Spline landmark warping
        if (parsed.count("disable-landmark-bspline") == 0) {
            // Update the landmark positions
            auto tfmLdm = graph.insertNode<TransformLandmarksNode>();
            affine->transform >> tfmLdm->transform;
            ldmNode->getOutputPort("movingLandmarks") >> tfmLdm->landmarksIn;

            // BSpline Warp
            auto bspline = graph.insertNode<BSplineLandmarkWarpingNode>();
            fixed->image >> bspline->fixedImage;
            ldmNode->getOutputPort("fixedLandmarks") >> bspline->fixedLandmarks;
            tfmLdm->landmarksOut >> bspline->movingLandmarks;
            bspline->transform >> landmarkTfms->second;
        }

        // Add landmark transforms to final transforms
        landmarkTfms->result >> compositeTfms->first;
    }

    ///// Deformable Registration /////
    if (parsed.count("disable-deformable") == 0) {
        // Resample moving image for next stage
        auto resample1 = graph.insertNode<ImageResampleNode>();
        fixed->image >> resample1->fixedImage;
        moving->image >> resample1->movingImage;
        landmarkTfms->result >> resample1->transform;

        // Compute deformable
        auto deformable = graph.insertNode<DeformableRegistrationNode>();
        deformable->iterations(parsed["deformable-iterations"].as<int>());
        fixed->image >> deformable->fixedImage;
        resample1->resampledImage >> deformable->movingImage;

        // Add transform to final composite
        deformable->transform >> compositeTfms->second;
    }

    ///// Apply the transformation to the UV map /////
    auto tfmUVs = graph.insertNode<TransformUVMapNode>();
    compositeTfms->result >> tfmUVs->transform;
    fixed->image >> tfmUVs->fixedImage;
    moving->image >> tfmUVs->movingImage;
    fixed->uvMap >> tfmUVs->uvMapIn;

    ///// Write output mesh /////
    auto writer = graph.insertNode<MeshWriteNode>();
    writer->path(outputPath);
    fixed->mesh >> writer->mesh;
    moving->image >> writer->image;
    tfmUVs->uvMapOut >> writer->uvMap;

    ///// Write the final transformations /////
    if (parsed.count("output-tfm") > 0) {
        auto tfmWriter = graph.insertNode<WriteTransformNode>();
        tfmWriter->path(parsed["output-tfm"].as<std::string>());
        compositeTfms->result >> tfmWriter->transform;
    }

    // Compute result
    graph.update();

    // Write Dot file
    if (parsed.count("output-dot") > 0) {
        smgl::WriteDotFile(parsed["output-dot"].as<std::string>(), graph);
    }

    return EXIT_SUCCESS;
}
