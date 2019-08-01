#include "rt/io/LandmarkReader.hpp"

#include <boost/algorithm/string.hpp>

using namespace rt;
namespace fs = boost::filesystem;

void LandmarkReader::read()
{
    // Check that input is set
    if (!fs::exists(landmarksPath_) || fixedImage_ == nullptr ||
        movingImage_ == nullptr) {
        throw std::runtime_error("Invalid input. Unable to read.");
    }

    // Reset landmark containers
    fixedLandmarks_.clear();
    movingLandmarks_.clear();

    // Read the landmarks file
    rt::Landmark fixedPoint, movingPoint;
    itk::ContinuousIndex<double, 2> fixedIndex, movingIndex;

    auto fixedSize = fixedImage_->GetLargestPossibleRegion().GetSize();
    auto movingSize = movingImage_->GetLargestPossibleRegion().GetSize();

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

        fixedIndex[0] = std::stod(strs[0]);
        fixedIndex[1] = std::stod(strs[1]);
        movingIndex[0] = std::stod(strs[2]);
        movingIndex[1] = std::stod(strs[3]);

        if (fixedIndex[0] < 0 || fixedIndex[1] < 0 ||
            fixedIndex[0] >= fixedSize[0] || fixedIndex[1] >= fixedSize[1]) {
            std::stringstream s;
            s << "Fixed landmark not in image bounds: " << fixedIndex;
            throw std::runtime_error(s.str());
        }

        if (movingIndex[0] < 0 || movingIndex[1] < 0 ||
            movingIndex[0] >= movingSize[0] ||
            movingIndex[1] >= movingSize[1]) {
            std::stringstream s;
            s << "Moving landmark not in image bounds: " << movingIndex;
            throw std::runtime_error(s.str());
        }

        // Transform landmarks in case spacing still gets used
        fixedImage_->TransformContinuousIndexToPhysicalPoint(
            fixedIndex, fixedPoint);
        movingImage_->TransformContinuousIndexToPhysicalPoint(
            movingIndex, movingPoint);

        fixedLandmarks_.push_back(fixedPoint);
        movingLandmarks_.push_back(movingPoint);
    }
    ifs.close();
}

void LandmarkReader::readRaw()
{
    // Check that input is set
    if (!fs::exists(landmarksPath_)) {
        throw std::runtime_error("Invalid input. Unable to read.");
    }

    // Reset landmark containers
    fixedLandmarks_.clear();
    movingLandmarks_.clear();

    // Read the landmarks file
    rt::Landmark fixedPoint, movingPoint;
    itk::ContinuousIndex<double, 2> fixedIndex, movingIndex;

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

        fixedIndex[0] = std::stod(strs[0]);
        fixedIndex[1] = std::stod(strs[1]);
        movingIndex[0] = std::stod(strs[2]);
        movingIndex[1] = std::stod(strs[3]);

        fixedLandmarks_.push_back(fixedIndex);
        movingLandmarks_.push_back(movingIndex);
    }
    ifs.close();
}