#include <iomanip>
#include <iostream>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "rt/DisegniSegmenter.hpp"
#include "rt/io/TIFFIO.hpp"

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    ///// Parse the command line options /////
    // clang-format off
    po::options_description required("General Options");
    required.add_options()
        ("help,h", "Show this message")
        ("input,i", po::value<std::string>()->required(),
            "Input disegni image")
        ("output-prefix", po::value<std::string>()->default_value("disegni_"),
            "Filename prefix for segmented images")
        ("output-format", po::value<std::string>()->default_value("png"),
            "Output image format")
        ("output-dir,o", po::value<std::string>(),
            "Output directory for segmented disegni images. "
            "Default: Current working directory")
        ("output-labels", po::value<std::string>(),
            "The file path to save the labels image (32bpc Mono")
        ("output-labels-rgb", po::value<std::string>(),
            "The file path to save the labels image (8bpc RGB");

    po::options_description preprocOpts("Preprocessing Options");
    preprocOpts.add_options()
        ("white-to-black", "Convert white pixels to black pixels")
        ("sharpen", "Apply Laplacian sharpen")
        ("blur", "Apply median blur");

    po::options_description segOpts("Segmentation Options");
    segOpts.add_options()
        ("bbox-buffer,b", po::value<int>()->default_value(10),
            "Number of pixels added to the bounding box of segmented objects");

    po::options_description all("Usage");
    all.add(required).add(preprocOpts).add(segOpts);
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
    // Load input
    fs::path inputPath = parsed["input"].as<std::string>();
    auto input = cv::imread(inputPath.string());
    if (input.empty()) {
        std::cout << "Could not open or find the image" << std::endl;
        return EXIT_FAILURE;
    }

    // Test data: hard-coded fg/bg pts
    std::vector<cv::Point> fgPts{{100, 78}};
    std::vector<cv::Point> bgPts{{10, 10}, {325, 425}};

    // Run segmenter
    std::cout << "Segmenting image..." << std::endl;
    rt::DisegniSegmenter segmenter;
    segmenter.setForegroundCoords(fgPts);
    segmenter.setBackgroundCoords(bgPts);

    segmenter.setInputImage(input);
    segmenter.setPreprocessWhiteToBlack(parsed.count("white-to-black") > 0);
    segmenter.setPreprocessSharpen(parsed.count("sharpen") > 0);
    segmenter.setPreprocessBlur(parsed.count("blur") > 0);
    segmenter.setBoundingBoxBuffer(parsed["bbox-buffer"].as<int>());
    auto results = segmenter.compute();

    // Setup output variables
    auto outDir = fs::current_path();
    if (parsed.count("output-dir")) {
        outDir = parsed["output-dir"].as<std::string>();
    }
    auto padding = std::to_string(results.size()).size();
    auto prefix = parsed["output-prefix"].as<std::string>();
    auto ext = "." + parsed["output-format"].as<std::string>();

    // Save the labels images
    if (parsed.count("output-labels") > 0) {
        std::cout << "Saving labels image..." << std::endl;
        fs::path labelPath = parsed["output-labels"].as<std::string>();
        labelPath.replace_extension("tif");
        rt::io::WriteTIFF(labelPath.string(), segmenter.getLabeledImage(false));
    }

    if (parsed.count("output-labels-rgb") > 0) {
        std::cout << "Saving RGB labels image..." << std::endl;
        cv::imwrite(
            parsed["output-labels-rgb"].as<std::string>(),
            segmenter.getLabeledImage(true));
    }

    // Save the subimages
    std::cout << "Saving disegni images..." << std::endl;
    size_t index = 0;
    for (const auto& r : results) {
        std::stringstream ss;
        ss << prefix << std::setw(padding) << std::setfill('0') << index << ext;
        cv::imwrite((outDir / ss.str()).string(), r);
        index++;
    }

    return EXIT_SUCCESS;
}
