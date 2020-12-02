#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>

#include <boost/program_options.hpp>

#include "rt/DisegniSegmenter.hpp"
#include "rt/filesystem.hpp"
#include "rt/io/ImageIO.hpp"

namespace fs = rt::filesystem;
namespace po = boost::program_options;

cv::Point ParsePointString(const std::string& s);

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
        ("output-format", po::value<std::string>()->default_value("tif"),
            "Output image format")
        ("output-dir,o", po::value<std::string>(),
            "Output directory for segmented disegni images. "
            "Default: Current working directory")
        ("output-labels", po::value<std::string>(),
            "The file path to save the labels image (32-bit "
            "integer labels)")
        ("output-labels-rgb", po::value<std::string>(),
            "The file path to save the labels image (RGB)");

    po::options_description preprocOpts("Preprocessing Options");
    preprocOpts.add_options()
        ("white-to-black", "Convert white pixels to black pixels")
        ("sharpen", "Apply Laplacian sharpen")
        ("blur", "Apply median blur");

    po::options_description segOpts("Segmentation Options");
    segOpts.add_options()
        ("seed-fg,f", po::value<std::vector<std::string>>()->required(),
            "Add a seed point for a foreground object. Should be a string in "
            "the format \"x,y\". May be specified multiple times.")
        ("seed-bg,b", po::value<std::vector<std::string>>()->required(),
            "Add a seed point for the background. Should be a string in "
            "the format \"x,y\". May be specified multiple times.")
        ("seed-size,s", po::value<int>()->default_value(1),
            "Radius of the seed points, in pixels")
        ("bbox-buffer", po::value<int>()->default_value(10),
            "Number of pixels added to the bounding box of segmented objects");

    po::options_description all("Usage");
    all.add(required).add(preprocOpts).add(segOpts);
    // clang-format on

    // Parse the cmd line
    po::variables_map parsed;
    po::store(po::command_line_parser(argc, argv).options(all).run(), parsed);

    // Show the help message
    if (parsed.count("help") || argc < 6) {
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
    auto input = rt::ReadImage(inputPath);
    if (input.empty()) {
        std::cout << "Could not open or find the image" << std::endl;
        return EXIT_FAILURE;
    }

    // Parse foreground seeds from opts
    auto fgStrs = parsed["seed-fg"].as<std::vector<std::string>>();
    std::vector<cv::Point> fgPts;
    for (const auto& f : fgStrs) {
        try {
            fgPts.push_back(ParsePointString(f));
        } catch (const std::exception& /* unused */) {
            std::cerr << "Warning: Could not parse string as point: ";
            std::cerr << f << std::endl;
        }
    }

    // Parse background seeds from opts
    auto bgStrs = parsed["seed-bg"].as<std::vector<std::string>>();
    std::vector<cv::Point> bgPts;
    for (const auto& b : bgStrs) {
        try {
            bgPts.push_back(ParsePointString(b));
        } catch (const std::exception& /* unused */) {
            std::cerr << "Warning: Could not parse string as point: ";
            std::cerr << b << std::endl;
        }
    }

    // Run segmenter
    std::cout << "Segmenting image..." << std::endl;
    rt::DisegniSegmenter segmenter;
    segmenter.setInputImage(input);
    segmenter.setForegroundSeeds(fgPts);
    segmenter.setBackgroundSeeds(bgPts);
    segmenter.setSeedSize(parsed["seed-size"].as<int>());
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
        rt::WriteImage(labelPath, segmenter.getLabeledImage(false));
    }

    if (parsed.count("output-labels-rgb") > 0) {
        std::cout << "Saving RGB labels image..." << std::endl;
        fs::path rgbLabelsPath = parsed["output-labels-rgb"].as<std::string>();
        rt::WriteImage(rgbLabelsPath, segmenter.getLabeledImage(true));
    }

    // Save the subimages
    std::cout << "Saving disegni images..." << std::endl;
    size_t index = 0;
    for (const auto& r : results) {
        std::stringstream ss;
        ss << prefix << std::setw(padding) << std::setfill('0') << index << ext;
        rt::WriteImage((outDir / ss.str()), r);
        index++;
    }

    return EXIT_SUCCESS;
}

cv::Point ParsePointString(const std::string& s)
{
    // Parse the string into doubles
    std::regex delim(",");
    std::sregex_token_iterator it(s.begin(), s.end(), delim, -1);
    std::vector<int> components;
    for (; it != std::sregex_token_iterator(); it++) {
        components.emplace_back(std::stoi(*it));
    }

    // Size check
    if (components.size() != 2) {
        throw std::invalid_argument(
            "String contains incorrect number of components: " +
            std::to_string(components.size()));
    }

    // Convert to point
    return {components[0], components[1]};
}