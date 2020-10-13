#include "rt/io/LandmarkReader.hpp"

#include <boost/algorithm/string.hpp>

using namespace rt;
namespace fs = boost::filesystem;

void LandmarkReader::read()
{
    // Check that input is set
    if (!fs::exists(landmarksPath_)) {
        throw std::runtime_error("Invalid input. Unable to read.");
    }

    // Reset landmark containers
    fixedLandmarks_.clear();
    movingLandmarks_.clear();

    // Read the landmarks file
    Landmark fixedLdm;
    Landmark movingLdm;

    std::ifstream ifs(landmarksPath_.string());
    std::string line;
    std::vector<std::string> strs;
    while (std::getline(ifs, line)) {
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

        fixedLandmarks_.push_back(fixedLdm);
        movingLandmarks_.push_back(movingLdm);
    }
    ifs.close();
}

LandmarkReader::LandmarkReader(boost::filesystem::path landmarksPath)
    : landmarksPath_(std::move(landmarksPath))
{
}

void LandmarkReader::setLandmarksPath(const boost::filesystem::path& path)
{
    landmarksPath_ = path;
}
rt::LandmarkContainer LandmarkReader::getFixedLandmarks()
{
    return fixedLandmarks_;
}
rt::LandmarkContainer LandmarkReader::getMovingLandmarks()
{
    return movingLandmarks_;
}
