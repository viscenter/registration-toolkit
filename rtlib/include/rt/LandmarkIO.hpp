#pragma once

#include <boost/filesystem.hpp>

#include "rt/ImageTypes.hpp"
#include "rt/LandmarkRegistration.hpp"

namespace rt
{
class LandmarkIO {
public:
    LandmarkIO(const boost::filesystem::path& landmarksPath)
        : landmarksPath_(landmarksPath)
    {
    }

    void setFixedImage(const Image8UC3::Pointer i) { fixedImage_ = i; }
    void setMovingImage(const Image8UC3::Pointer i) { movingImage_ = i; }
    void setLandmarksPath(const boost::filesystem::path& path)
    {
        landmarksPath_ = path;
    }

    void read();

    LandmarkRegistration::LandmarkContainer getFixedLandmarks()
    {
        return fixedLandmarks_;
    }

    LandmarkRegistration::LandmarkContainer getMovingLandmarks()
    {
        return movingLandmarks_;
    }

private:
    boost::filesystem::path landmarksPath_;
    Image8UC3::Pointer fixedImage_;
    Image8UC3::Pointer movingImage_;

    LandmarkRegistration::LandmarkContainer fixedLandmarks_;
    LandmarkRegistration::LandmarkContainer movingLandmarks_;
};
}