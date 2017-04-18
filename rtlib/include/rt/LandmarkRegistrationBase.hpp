#pragma once

#include <itkPoint.h>

namespace rt
{
using Landmark = itk::Point<double, 2>;
using LandmarkContainer = std::vector<Landmark>;

class LandmarkRegistrationBase
{
public:
    void setFixedLandmarks(const LandmarkContainer& l) { fixedLdmks_ = l; }
    void setMovingLandmarks(const LandmarkContainer& l) { movingLdmks_ = l; }

protected:
    LandmarkRegistrationBase() = default;

    LandmarkContainer fixedLdmks_;
    LandmarkContainer movingLdmks_;
};
}
