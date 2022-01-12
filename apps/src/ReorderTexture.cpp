#include <string>
#include <unordered_map>

#include <boost/program_options.hpp>
#include <opencv2/imgproc.hpp>

#include "rt/ReorderUnorganizedTexture.hpp"
#include "rt/filesystem.hpp"
#include "rt/io/OBJReader.hpp"
#include "rt/io/OBJWriter.hpp"
#include "rt/types/ITKMesh.hpp"
#include "rt/util/String.hpp"

namespace fs = rt::filesystem;
namespace po = boost::program_options;

using namespace rt;

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

    po::options_description all("Usage");
    all.add(required);
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

    // Load the mesh
    std::cerr << "Reading mesh: " << inputPath << "\n";
    io::OBJReader reader;
    reader.setPath(inputPath);
    auto mesh = reader.read();
    auto uvMap = reader.getUVMap();
    auto texture = reader.getTextureMat();

    // We don't support RGBA textures
    auto channels = texture.channels();
    if (channels == 4) {
        cv::cvtColor(texture, texture, cv::COLOR_BGRA2BGR);
    } else if (channels != 1 && channels != 3) {
        std::cerr << "Texture has unsupported channels: " << channels << "\n";
    }

    // Reorder the texture
    std::cerr << "Reordering texture...\n";
    ReorderUnorganizedTexture r;
    r.setMesh(mesh);
    r.setUVMap(uvMap);
    r.setTextureMat(texture);
    r.setSamplingOrigin(samplingOrigin);
    r.setSamplingMode(sampleMode);
    r.setSampleRate(sampleRate);
    r.setSampleDim(sampleDim);
    r.setUseFirstIntersection(useFirstIntersection);
    r.compute();

    // Write to file
    io::OBJWriter writer;
    writer.setPath(outputPath);
    writer.setMesh(mesh);
    writer.setUVMap(r.getUVMap());
    writer.setTexture(r.getTextureMat());
    writer.write();
}
