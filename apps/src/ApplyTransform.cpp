#include <iostream>

#include <boost/program_options.hpp>

#include "rt/ImageTransformResampler.hpp"
#include "rt/filesystem.hpp"
#include "rt/io/ImageIO.hpp"
#include "rt/types/Transforms.hpp"
#include "rt/util/ImageConversion.hpp"

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
            ("transform,t", po::value<std::string>()->required(),
                "Input file path for the transform file")
            ("output-file,o", po::value<std::string>()->required(),
                "Output file path for the registered moving image")
            ("enable-alpha", "If enabled, an alpha layer will be "
                "added to the moving image if it does not already have one.");

    po::options_description all("Usage");
    all.add(required);
    // clang-format on

    // Parse the cmd line
    po::variables_map parsed;
    po::store(po::command_line_parser(argc, argv).options(all).run(), parsed);

    // Show the help message
    if ((parsed.count("help") > 0) || argc < 4) {
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
    fs::path tfmPath = parsed["transform"].as<std::string>();
    fs::path outputPath = parsed["output-file"].as<std::string>();

    // Read transform
    auto transform = rt::ReadTransform(tfmPath);

    // Load the fixed image and moving image (at full depth)
    auto fixed = rt::ReadImage(fixedPath);
    auto moving = rt::ReadImage(movingPath);

    // Add alpha channel if requested and needed
    if (parsed.count("enable-alpha") > 0 and
        (moving.channels() == 1 or moving.channels() == 3)) {
        moving = rt::ColorConvertImage(moving, moving.channels() + 1);
    }

    // Transform image
    std::cout << "Transforming image..." << std::endl;
    auto final = rt::ImageTransformResampler(moving, fixed.size(), transform);

    // Write out the file
    std::cout << "Writing transformed image..." << std::endl;
    rt::WriteImage(outputPath, final);
}