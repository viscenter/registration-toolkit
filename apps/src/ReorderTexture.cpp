#include <boost/program_options.hpp>
#include <opencv2/imgproc.hpp>

#include "rt/ReorderUnorganizedTexture.hpp"
#include "rt/filesystem.hpp"
#include "rt/io/OBJReader.hpp"
#include "rt/io/OBJWriter.hpp"
#include "rt/types/ITKMesh.hpp"

namespace fs = rt::filesystem;
namespace po = boost::program_options;

int main(int argc, char* argv[])
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
        ("sample-rate,r", po::value<double>()->default_value(0.1),
             "Sample rate at which mesh space is rasterized to pixels")
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
    if (parsed.count("help") || argc < 5) {
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
    auto sampleRate = parsed["sample-rate"].as<double>();
    auto useFirstIntersection = parsed.count("use-first-intersection") > 0;

    // Load the mesh
    std::cerr << "Reading mesh: " << inputPath << "\n";
    rt::io::OBJReader reader;
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
    std::cerr << "Reordering texture :: Sample Rate: " << sampleRate << "\n";
    rt::ReorderUnorganizedTexture r;
    r.setMesh(mesh);
    r.setUVMap(uvMap);
    r.setTextureMat(texture);
    r.setSampleRate(sampleRate);
    r.setUseFirstIntersection(useFirstIntersection);
    r.compute();

    // Write to file
    rt::io::OBJWriter writer;
    writer.setPath(outputPath);
    writer.setMesh(mesh);
    writer.setUVMap(r.getUVMap());
    writer.setTexture(r.getTextureMat());
    writer.write();
}
