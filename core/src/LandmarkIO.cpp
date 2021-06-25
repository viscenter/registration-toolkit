#include "rt/io/LandmarkIO.hpp"

#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

#include "rt/types/Exceptions.hpp"
#include "rt/util/String.hpp"

using namespace rt;
namespace fs = rt::filesystem;

void LandmarkWriter::setPath(const fs::path& p) { path_ = p; }

void LandmarkWriter::setFixedLandmarks(const LandmarkContainer& f)
{
    fixed_ = f;
}

void LandmarkWriter::setMovingLandmarks(const LandmarkContainer& m)

{
    moving_ = m;
}

void LandmarkWriter::write()
{
    // Check parameters
    if (path_.empty()) {
        throw std::runtime_error("Empty file path");
    }

    if (fixed_.size() != moving_.size()) {
        throw std::runtime_error("Landmark containers of different size");
    }

    // Open file
    std::ofstream file(path_.string());
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open output file");
    }

    // Write the landmarks
    for (size_t i = 0; i < fixed_.size(); i++) {
        file << fixed_.at(i)[0] << " ";
        file << fixed_.at(i)[1] << " ";
        file << moving_.at(i)[0] << " ";
        file << moving_.at(i)[1] << "\n";
    }

    // Close file
    file.close();
}

void LandmarkReader::read()
{
    // Check that input is set
    if (!fs::exists(path_)) {
        throw std::runtime_error("Invalid input. Unable to read.");
    }

    // Reset landmark containers
    fixed_.clear();
    moving_.clear();

    // Read the landmarks file
    Landmark fixedLdm;
    Landmark movingLdm;

    std::ifstream ifs(path_.string());
    std::string line;
    while (std::getline(ifs, line)) {
        // Remove comments
        line = line.substr(0, line.find('#', 0));
        if (line.empty()) {
            continue;
        }

        // Parse the line
        trim(line);
        auto strs = split(line);
        std::for_each(
            std::begin(strs), std::end(strs), [](std::string& t) { trim(t); });

        if (strs.size() != 4) {
            continue;
        }

        fixedLdm[0] = std::stod(strs[0]);
        fixedLdm[1] = std::stod(strs[1]);
        movingLdm[0] = std::stod(strs[2]);
        movingLdm[1] = std::stod(strs[3]);

        fixed_.push_back(fixedLdm);
        moving_.push_back(movingLdm);
    }
    ifs.close();
}

LandmarkReader::LandmarkReader(fs::path landmarksPath)
    : path_(std::move(landmarksPath))
{
}

void LandmarkReader::setLandmarksPath(const fs::path& path) { path_ = path; }

auto LandmarkReader::getFixedLandmarks() -> rt::LandmarkContainer
{
    return fixed_;
}

auto LandmarkReader::getMovingLandmarks() -> rt::LandmarkContainer
{
    return moving_;
}

void rt::WriteLandmarkContainer(
    const fs::path& path, const LandmarkContainer& lc)
{
    // Open file
    std::ofstream file(path.string(), std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open output file");
    }

    // Make header
    std::stringstream ss;
    ss << "filetype: LandmarkContainer" << std::endl;
    ss << "size: " << lc.size() << std::endl;
    ss << "dim: 2" << std::endl;
    ss << "type: double" << std::endl;
    ss << "<>" << std::endl;

    auto str = ss.str();
    file.write(str.c_str(), str.size());

    // Write the landmarks
    auto bytes = 2 * sizeof(double);
    for (const auto& l : lc) {
        file.write(reinterpret_cast<const char*>(l.GetDataPointer()), bytes);
    }

    // Close file
    file.close();
}
auto rt::ReadLandmarkContainer(const fs::path& path) -> LandmarkContainer
{
    // Open file
    std::ifstream file{path.string(), std::ios::binary};
    if (!file.is_open()) {
        throw std::runtime_error("Invalid input. Unable to read.");
    }

    struct Header {
        std::string filetype;
        size_t size{0};
        size_t dim{0};
        std::string type;
    };

    // Parse header
    Header h;
    const std::regex filetype{"^filetype"};
    const std::regex size{"^size"};
    const std::regex dim{"^dim"};
    const std::regex type{"^type"};
    const std::regex terminator{"^<>$"};

    std::string line;
    while (std::getline(file, line)) {
        // Remove comments
        line = line.substr(0, line.find('#', 0));
        if (line.empty()) {
            continue;
        }

        // Tokenize the line
        trim(line);
        auto strs = split(line, ':');
        std::for_each(
            std::begin(strs), std::end(strs), [](auto& t) { trim(t); });

        // Filetype
        if (std::regex_match(strs[0], filetype)) {
            h.filetype = strs[1];
        }
        // Size
        else if (std::regex_match(strs[0], size)) {
            h.size = std::stoull(strs[1]);
        }
        // Dimensions
        else if (std::regex_match(strs[0], dim)) {
            h.dim = std::stoull(strs[1]);
        }
        // Type
        else if (std::regex_match(strs[0], type)) {
            h.type = strs[1];
        }
        // End of the header
        else if (std::regex_match(line, terminator)) {
            break;
        }
        // Ignore everything else
        else {
            continue;
        }
    }

    // Validate header
    if (h.filetype.empty()) {
        throw IOException("File missing filetype field");
    } else if (h.filetype != "LandmarkContainer") {
        throw IOException("Filetype is not LandmarkContainer: " + h.filetype);
    } else if (h.type != "double") {
        throw IOException("Unsupported element type: " + h.type);
    } else if (h.dim != 2) {
        throw IOException(
            "Unsupported num. of dimensions: " + std::to_string(h.dim));
    }

    // Read data
    LandmarkContainer lc;
    Landmark l;
    for (size_t i = 0; i < h.size; i++) {
        std::ignore = i;
        file.read(
            reinterpret_cast<char*>(l.GetDataPointer()), 2 * sizeof(double));
        lc.push_back(l);
    }

    return lc;
}
