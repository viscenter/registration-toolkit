#pragma once

/** @file */

#include <itkPoint.h>

namespace rt
{
/** @brief Landmark registration landmark
 */
using Landmark = itk::Point<double, 2>;

/** @brief Generic Landmark container
 */
using LandmarkContainer = std::vector<Landmark>;

/**
 * @class LandmarkRegistrationBase
 * @brief Base class for landmark registration algorithms
 */
class LandmarkRegistrationBase
{
public:
    /** @brief Set the fixed landmarks */
    void setFixedLandmarks(const LandmarkContainer& l) { fixedLdmks_ = l; }

    /** @brief Set the moving landmarks */
    void setMovingLandmarks(const LandmarkContainer& l) { movingLdmks_ = l; }

protected:
    /** Default constructor */
    LandmarkRegistrationBase() = default;

    /** Fixed landmarks container */
    LandmarkContainer fixedLdmks_;

    /** Moving landmarks container */
    LandmarkContainer movingLdmks_;
};
}
