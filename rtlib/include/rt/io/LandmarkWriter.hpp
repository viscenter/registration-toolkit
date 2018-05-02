#pragma once

#include <boost/filesystem.hpp>

#include "rt/LandmarkRegistrationBase.hpp"

namespace rt
{

class LandmarkWriter
{
public:
    LandmarkWriter() = default;
    void setPath(boost::filesystem::path p) { outputPath_ = p; }
    void setFixedLandmarks(LandmarkContainer f) { fixedLandmarks_ = f; }
    void setMovingLandmarks(LandmarkContainer m) { movingLandmarks_ = m; }

    void write();

private:
    boost::filesystem::path outputPath_;

    LandmarkContainer fixedLandmarks_;
    LandmarkContainer movingLandmarks_;
};
}  // namespace rt