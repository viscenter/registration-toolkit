#include "rt/io/LandmarkReader.hpp"

#include <string>

#include <boost/algorithm/string.hpp>

using namespace rt;
namespace fs = boost::filesystem;

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
    std::vector<std::string> strs;
    while (std::getline(ifs, line)) {
        // Remove comments
        line = line.substr(0, line.find('#', 0));
        if (line.empty()) {
            continue;
        }

        // Parse the line
        boost::trim(line);
        boost::split(strs, line, boost::is_any_of(" "));
        std::for_each(std::begin(strs), std::end(strs), [](std::string& t) {
            boost::trim(t);
        });

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

LandmarkReader::LandmarkReader(boost::filesystem::path landmarksPath)
    : path_(std::move(landmarksPath))
{
}

void LandmarkReader::setLandmarksPath(const boost::filesystem::path& path)
{
    path_ = path;
}
rt::LandmarkContainer LandmarkReader::getFixedLandmarks() { return fixed_; }
rt::LandmarkContainer LandmarkReader::getMovingLandmarks() { return moving_; }
