#include <smgl/Graph.hpp>
#include <smgl/Graphviz.hpp>

#include "rt/filesystem.hpp"
#include "rt/graph/Nodes.hpp"

using namespace rt;
using namespace rt::graph;
using namespace smgl;

namespace fs = rt::filesystem;

smgl::GraphStyle style;

struct SubgraphOutput {
    OutputPort<cv::Mat>* inputImage{nullptr};
    OutputPort<cv::Mat>* outputImage{nullptr};
    OutputPort<Transform::Pointer>* transform{nullptr};
};

// Warning: SubgraphOutput is only valid for the lifetime of the passed Graph
static SubgraphOutput AddImageRegSubgraph(
    Graph& graph,
    const fs::path& movingPath,
    OutputPort<cv::Mat>& fixedImage,
    const fs::path& ldmsPath = "",
    bool runBspline = true,
    bool runDeformable = true,
    bool genResample = false)
{
    // Setup the output struct
    SubgraphOutput result;

    // Read image
    auto readIm = graph.insertNode<ImageReadNode>();
    readIm->path = movingPath;
    result.inputImage = &readIm->image;
    style.setRankSource(readIm);

    // Detect/Load landmarks
    Node::Pointer ldmNode;
    if (ldmsPath.empty()) {
        auto detLdm = graph.insertNode<LandmarkDetectorNode>();
        detLdm->fixedImage = fixedImage;
        detLdm->movingImage = readIm->image;
        ldmNode = detLdm;
    } else {
        auto loadLdm = graph.insertNode<LandmarkReaderNode>();
        loadLdm->path = ldmsPath;
        ldmNode = loadLdm;
    }

    // Calculate affine transform
    auto affLdm = graph.insertNode<AffineLandmarkRegistrationNode>();
    affLdm->fixedLandmarks = ldmNode->getOutputPort("fixedLandmarks");
    affLdm->movingLandmarks = ldmNode->getOutputPort("movingLandmarks");
    result.transform = &affLdm->transform;

    if (runBspline) {
        // Update the landmark positions
        auto updateLdm = graph.insertNode<TransformLandmarksNode>();
        updateLdm->transform = affLdm->transform;
        updateLdm->landmarksIn = ldmNode->getOutputPort("movingLandmarks");

        // Calculate bspline warping
        auto bspLdm = graph.insertNode<BSplineLandmarkWarpingNode>();
        bspLdm->fixedImage = fixedImage;
        bspLdm->fixedLandmarks = ldmNode->getOutputPort("fixedLandmarks");
        bspLdm->movingLandmarks = updateLdm->landmarksOut;

        // Combine the landmark transforms
        auto ldmTfm = graph.insertNode<CompositeTransformNode>();
        ldmTfm->first = affLdm->transform;
        ldmTfm->second = bspLdm->transform;
        result.transform = &ldmTfm->result;
    }

    // Run deformable as requested
    if (runDeformable) {
        // Resample the input for deformable
        auto resIm1 = graph.insertNode<ImageResampleNode>();
        resIm1->fixedImage = fixedImage;
        resIm1->movingImage = readIm->image;
        resIm1->transform = *result.transform;

        // Run deformable
        auto defReg = graph.insertNode<DeformableRegistrationNode>();
        defReg->fixedImage = fixedImage;
        defReg->movingImage = resIm1->resampledImage;
        defReg->iterations = 25;

        // Final transform
        auto allTfms = graph.insertNode<CompositeTransformNode>();
        allTfms->first = *result.transform;
        allTfms->second = defReg->transform;
        result.transform = &allTfms->result;
    }

    // Resample the output as requested
    if (genResample) {
        auto resIm2 = graph.insertNode<ImageResampleNode>();
        resIm2->fixedImage = fixedImage;
        resIm2->movingImage = readIm->image;
        resIm2->transform = *result.transform;
        result.outputImage = &resIm2->resampledImage;
    }

    return result;
}

static void WriteRetextureMeshSubgraph(
    Graph& graph,
    const fs::path& path,
    OutputPort<ITKMesh::Pointer>& mesh,
    OutputPort<UVMap>& uvMap,
    OutputPort<cv::Mat>& texture)
{
    auto writer = graph.insertNode<MeshWriteNode>();
    writer->path = path;
    writer->mesh = mesh;
    writer->uvMap = uvMap;
    writer->image = texture;
    style.setRankSink(writer);
}

int main()
{
    // Register node types for serialization
    RegisterAllNodeTypes();

    std::string project = "PHerc118-Pezzo4";
    bool useCache = true;

    // Setup graph
    Graph graph;
    graph.setEnableCache(useCache);
    graph.setCacheFile("results/" + project + ".json");
    graph.setCacheType(CacheType::Subdirectory);

    // Clean cache for repeated runs
    fs::remove_all(graph.cacheDir());

    // Setup output dir
    fs::path outDir = "results/" + project;
    fs::create_directories(outDir);

    // Load mesh
    auto meshRead = graph.insertNode<MeshReadNode>();
    meshRead->path = "2017/PHerc118-Pezzo4-Artec4.obj";
    style.setRankSource(meshRead);

    // Reorder texture
    auto reorder = graph.insertNode<ReorderTextureNode>();
    reorder->meshIn = meshRead->mesh;
    reorder->uvMapIn = meshRead->uvMap;
    reorder->imageIn = meshRead->image;
    reorder->sampleRate = 0.1;

    // Reg 1998
    auto result1998 = AddImageRegSubgraph(
        graph, "1998/bodley_grclassb1_Pezzo04.tif", reorder->imageOut,
        "1998/1998-to-2017-landmarks.ldm");
    auto write1998tfm = graph.insertNode<WriteTransformNode>();
    write1998tfm->path = "PHerc118-1998-to-Mesh2017.tfm";
    write1998tfm->transform = *result1998.transform;

    // Update the UV map to point to 1998 space
    auto transformUV1998 = graph.insertNode<TransformUVMapNode>();
    transformUV1998->uvMapIn = reorder->uvMapOut;
    transformUV1998->fixedImage = reorder->imageOut;
    transformUV1998->movingImage = *result1998.inputImage;
    transformUV1998->transform = *result1998.transform;

    // Reg 2005 -> 1998
    auto result2005 = AddImageRegSubgraph(
        graph, "2005/Bod05b-PHerc118c04-0950-40b.tif", *result1998.inputImage,
        "2005/2005-to-1998-landmarks.ldm");
    auto write2005tfm = graph.insertNode<WriteTransformNode>();
    write2005tfm->path = "PHerc118-2005-to-1998.tfm";
    write2005tfm->transform = *result2005.transform;

    // Update the UV map to point to 2005 space
    auto tfm2005ToReorder = graph.insertNode<CompositeTransformNode>();
    tfm2005ToReorder->first = *result2005.transform;
    tfm2005ToReorder->second = *result1998.transform;
    auto write2005tfm2 = graph.insertNode<WriteTransformNode>();
    write2005tfm2->path = "PHerc118-2005-to-Mesh2017.tfm";
    write2005tfm2->transform = tfm2005ToReorder->result;
    style.setRankSink(write1998tfm, write2005tfm, write2005tfm2);

    auto transformUV2005 = graph.insertNode<TransformUVMapNode>();
    transformUV2005->uvMapIn = reorder->uvMapOut;
    transformUV2005->fixedImage = reorder->imageOut;
    transformUV2005->movingImage = *result2005.inputImage;
    transformUV2005->transform = tfm2005ToReorder->result;

    // Load 2017 IR/RGB (preregistered to 1998)
    auto readIR2017 = graph.insertNode<ImageReadNode>();
    readIR2017->path = "2017/PHerc118-Pezzo4-IR950.png";
    auto readRGB2017 = graph.insertNode<ImageReadNode>();
    readRGB2017->path = "2017/PHerc118-Pezzo4-RGB2020_8bpc.png";
    style.setRankSource(readIR2017, readRGB2017);

    // Register disegni to 1998
    fs::path disegniDir = "Disegni/Napolitani-individual";
    std::size_t cnt{0};
    for (const auto& e : fs::directory_iterator(disegniDir)) {
        if (cnt == 2) {
            break;
        }
        const auto& imgPath = e.path();
        if (not fs::is_regular_file(imgPath) or imgPath.extension() != ".png") {
            continue;
        }

        auto ldmPath = imgPath;
        ldmPath.replace_extension("ldm");
        if (not fs::exists(ldmPath)) {
            continue;
        }

        auto r = AddImageRegSubgraph(
            graph, imgPath, *result2005.inputImage, ldmPath, true, false, true);
        auto writeDisegnitfm = graph.insertNode<WriteTransformNode>();
        writeDisegnitfm->path = imgPath.stem().string() + "-to-Mesh2017.tfm";
        writeDisegnitfm->transform = *r.transform;

        fs::create_directories(outDir / "Disegni-overlays-2005");
        auto outFile = imgPath.filename().replace_extension("tif");
        auto outputPath = outDir / "Disegni-overlays-2005" / outFile;
        auto writer = graph.insertNode<ImageWriteNode>();
        writer->path = outputPath;
        writer->image = *r.outputImage;
        style.setRankSink(writeDisegnitfm, writer);

        cnt++;
    }

    // Write meshes
    std::string prefix = "PHerc118-Pezzo04-";
    WriteRetextureMeshSubgraph(
        graph, outDir / (prefix + "1998.obj"), meshRead->mesh,
        transformUV1998->uvMapOut, *result1998.inputImage);
    WriteRetextureMeshSubgraph(
        graph, outDir / (prefix + "2005.obj"), meshRead->mesh,
        transformUV2005->uvMapOut, *result2005.inputImage);
    WriteRetextureMeshSubgraph(
        graph, outDir / (prefix + "2017-IR950.obj"), meshRead->mesh,
        transformUV1998->uvMapOut, readIR2017->image);
    WriteRetextureMeshSubgraph(
        graph, outDir / (prefix + "2017-RGB.obj"), meshRead->mesh,
        transformUV1998->uvMapOut, readRGB2017->image);

    // Setup style
    style.defaultStyle().inputPorts.bgcolor = "#87BF73";
    style.defaultStyle().inputPorts.color = "#87BF73";
    style.defaultStyle().label.bgcolor = "#1897D4";
    style.defaultStyle().label.color = "#1897D4";
    style.defaultStyle().outputPorts.bgcolor = "#4C9D2F";
    style.defaultStyle().outputPorts.color = "#4C9D2F";
    style.defaultStyle().font.color = "white";

    NodeStyle doStyle;
    doStyle.label.bgcolor = "#0033A0";
    doStyle.label.color = "#0033A0";
    style.setClassStyle<ImageReadNode>(doStyle);
    style.setClassStyle<MeshReadNode>(doStyle);
    style.setClassStyle<ImageWriteNode>(doStyle);
    style.setClassStyle<MeshWriteNode>(doStyle);
    style.setClassStyle<WriteTransformNode>(doStyle);

    // Run the graph
    smgl::WriteDotFile("results/" + project + "-graph.gv", graph, style);
    graph.update();
    smgl::WriteDotFile("results/" + project + "-graph.gv", graph, style);
}