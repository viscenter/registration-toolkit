#include <string>
#include <unordered_map>

#include <boost/program_options.hpp>
#include <smgl/Graphviz.hpp>
#include <smgl/smgl.hpp>

#include "rt/Version.hpp"
#include "rt/filesystem.hpp"
#include "rt/graph.hpp"
#include "rt/util/String.hpp"

namespace fs = rt::filesystem;
namespace po = boost::program_options;

using namespace rt;
using namespace rt::graph;

using SamplingOrigin = ReorderUnorganizedTexture::SamplingOrigin;
std::unordered_map<std::string, SamplingOrigin> StrToOrigin{
    {"tl", SamplingOrigin::TopLeft},
    {"tr", SamplingOrigin::TopRight},
    {"bl", SamplingOrigin::BottomLeft},
    {"br", SamplingOrigin::BottomRight},
};

using SamplingMode = ReorderUnorganizedTexture::SamplingMode;
std::unordered_map<std::string, SamplingMode> StrToMode{
    {"rate", SamplingMode::Rate},
    {"width", SamplingMode::OutputWidth},
    {"height", SamplingMode::OutputHeight},
    {"auto", SamplingMode::AutoUV},
};

auto main(int argc, char* argv[]) -> int
{
    ///// Parse the command line options /////
    // clang-format off
    po::options_description required("General Options");
    required.add_options()
        ("help,h", "Show this message")
        ("input-mesh,i", po::value<std::string>()->required(),
             "Path to input OBJ with unordered texture (i.e. multicharts)")
        ("output-mesh,o", po::value<std::string>()->required(),
             "Path to output OBJ with ordered texture")
        ("sampling-origin", po::value<std::string>()->default_value("tl"),
             "Origins: tl, tr, bl, br")
        ("sampling-mode,m", po::value<std::string>()->default_value("auto"),
             "Modes: rate, width, height, auto. If 'rate', specify "
             "the sampling step size in mesh units using --sampling-rate. If "
             "'width' or 'height', specify the length of the corresponding "
             "dimension in pixels using --sampling-dim. If 'auto', the sample "
             "automatically calculated from the average pixel density of the "
             "input mesh.")
        ("sampling-rate,r", po::value<double>()->default_value(0.1),
             "If --sampling-mode is 'rate', the pixel size in mesh units")
        ("sampling-dim,d", po::value<std::size_t>()->default_value(800),
             "If --sampling-mode is 'width' or 'height', the length of the "
             "corresponding output dimension in pixels")
        ("use-first-intersection,f", "This program assumes that "
             "the projection origin is behind the base plane of the sampled "
             "mesh. Thus, the last mesh intersection point will lie on the "
             "visible surface. If instead the projection origin is in front of "
             "the base plane, the first mesh intersection point lies on the "
             "visible surface.");

    po::options_description graphOptions("Render Graph Options");
    graphOptions.add_options()
    ("output-graph,g", po::value<std::string>(), "Render graph JSON file")
    ("output-dot", po::value<std::string>(), "Render graph Dot file");

    po::options_description all("Usage");
    all.add(required).add(graphOptions);
    // clang-format on

    // Parse the cmd line
    po::variables_map parsed;
    po::store(po::command_line_parser(argc, argv).options(all).run(), parsed);

    // Show the help message
    if (parsed.count("help") > 0 || argc < 5) {
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

    fs::path inputPath = parsed["input-mesh"].as<std::string>();
    fs::path outputPath = parsed["output-mesh"].as<std::string>();

    // Get parameters
    auto originStr = to_lower_copy(parsed["sampling-origin"].as<std::string>());
    auto samplingOrigin = StrToOrigin.at(originStr);
    auto modeStr = to_lower_copy(parsed["sampling-mode"].as<std::string>());
    auto sampleMode = StrToMode.at(modeStr);
    auto sampleRate = parsed["sampling-rate"].as<double>();
    auto sampleDim = parsed["sampling-dim"].as<std::size_t>();
    auto useFirstIntersection = parsed.count("use-first-intersection") > 0;

    ///// Start render graph /////
    rt::graph::RegisterNodes();
    smgl::Graph graph;

    // Add the project metadata
    graph.setProjectMetadata({{ProjectInfo::Name(), ProjectMetadata()}});

    ///// Setup caching /////
    if (parsed.count("output-graph") > 0) {
        fs::path cacheFile = parsed["output-graph"].as<std::string>();
        graph.setEnableCache(true);
        graph.setCacheFile(cacheFile);
    }

    // Load the mesh
    auto reader = graph.insertNode<MeshReadNode>();
    reader->path = inputPath;

    // We don't support RGBA textures
    auto convert = graph.insertNode<ColorConvertNode>();
    convert->imageIn = reader->image;
    convert->channels = 3;

    // Reorder the texture
    auto reorder = graph.insertNode<ReorderTextureNode>();
    reorder->meshIn = reader->mesh;
    reorder->uvMapIn = reader->uvMap;
    reorder->imageIn = convert->imageOut;
    reorder->samplingOrigin = samplingOrigin;
    reorder->samplingMode = sampleMode;
    reorder->sampleRate = sampleRate;
    reorder->sampleDim = sampleDim;
    reorder->useFirstIntersection = useFirstIntersection;

    // Write to file
    auto writer = graph.insertNode<MeshWriteNode>();
    writer->path = outputPath;
    writer->mesh = reader->mesh;
    writer->uvMap = reorder->uvMapOut;
    writer->image = reorder->imageOut;

    // Compute result
    graph.update();

    // Write Dot file
    if (parsed.count("output-dot") > 0) {
        smgl::WriteDotFile(parsed["output-dot"].as<std::string>(), graph);
    }
}
