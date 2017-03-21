#pragma once

#include <boost/filesystem.hpp>

#include "rt/ImageTypes.hpp"
#include "rt/LandmarkRegistration.hpp"

namespace rt
{
class LandmarkIO {
public:
    LandmarkIO() {}

    LandmarkIO(const boost::filesystem::path& landmarksPath)
        : landmarksPath_(landmarksPath)
    {
    }

    void setFixedImage(const Image8UC3::Pointer image) { fixedImage_ = image; }
    void setMovingImage(const Image8UC3::Pointer image)
    {
        movingImage_ = image;
    }
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
    Image8UC3::Pointer fixedImage_;
    Image8UC3::Pointer movingImage_;
    boost::filesystem::path landmarksPath_;
    rt::LandmarkRegistration::LandmarkContainer fixedLandmarks_;
    rt::LandmarkRegistration::LandmarkContainer movingLandmarks_;
};
}