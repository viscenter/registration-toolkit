#include <iostream>

#include <boost/program_options.hpp>

#include "rt/filesystem.hpp"
#include "rt/io/LandmarkIO.hpp"

using namespace rt;

namespace po = boost::program_options;
namespace fs = rt::filesystem;

int main(int argc, char* argv[])
{
    ///// Parse the command line options /////
    // clang-format off
    po::options_description required("General Options");
    required.add_options()
        ("help,h", "Show this message")
        ("input-landmarks,i", po::value<std::string>()->required(),
         "Input landmarks file")
        ("output-landmarks,o", po::value<std::string>()->required(),
         "Output landmarks file");

    po::options_description all("Usage");
    all.add(required);
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

    fs::path inputPath = parsed["input-landmarks"].as<std::string>();
    fs::path outputPath = parsed["output-landmarks"].as<std::string>();

    ///// Read input file /////
    LandmarkReader reader;
    reader.setLandmarksPath(inputPath);
    reader.read();

    // Swap landmarks
    auto movingLandmarks = reader.getFixedLandmarks();
    auto fixedLandmarks = reader.getMovingLandmarks();

    // Write landmarks
    LandmarkWriter writer;
    writer.setPath(outputPath);
    writer.setFixedLandmarks(fixedLandmarks);
    writer.setMovingLandmarks(movingLandmarks);
    writer.write();
}
