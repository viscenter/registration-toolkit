#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "rt/io/OBJReader.hpp"
#include "rt/io/OBJWriter.hpp"

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int main(int argc, char** argv)
{
    ///// Parse the command line options /////
    // All command line options
    // clang-format off
    po::options_description required("General Options");
    required.add_options()
        ("help,h", "Show this message")
        ("input-mesh,i", po::value<std::string>()->required(), "Input mesh file")
        ("texture,t", po::value<std::string>()->required(), "New texture image")
        ("output-mesh,o", po::value<std::string>()->required(), "Output mesh file");

    po::options_description all("Usage");
    all.add(required);
    // clang-format on

    // Parse the cmd line
    po::variables_map parsed;
    po::store(po::command_line_parser(argc, argv).options(all).run(), parsed);

    // Show the help message
    if (parsed.count("help") || argc < 2) {
        std::cout << all << std::endl;
        return EXIT_SUCCESS;
    }

    // Warn of missing options
    try {
        po::notify(parsed);
    } catch (po::error& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Load mesh
    fs::path inputPath = parsed["input-mesh"].as<std::string>();
    rt::io::OBJReader reader;
    reader.setPath(inputPath);
    reader.read();

    // Load the image
    fs::path imagePath = parsed["texture"].as<std::string>();
    auto image = cv::imread(imagePath.string(), -1);

    // Write the new mesh
    fs::path outputPath = parsed["output-mesh"].as<std::string>();
    rt::io::OBJWriter writer;
    writer.setPath(outputPath);
    writer.setMesh(reader.getMesh());
    writer.setUVMap(reader.getUVMap());
    writer.setTexture(image);
    writer.write();

    return EXIT_SUCCESS;
}
