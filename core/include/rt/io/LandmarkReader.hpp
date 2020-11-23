#pragma once

/** @file */

#include <opencv2/core.hpp>

#include "rt/LandmarkRegistrationBase.hpp"
#include "rt/filesystem.hpp"

namespace rt
{
/**
 * @class LandmarkReader
 * @author Zack Anderson
 * @brief Landmarks file reader
 *
 */
class LandmarkReader
{
public:
    /** @brief Default constructor */
    LandmarkReader() = default;

    /** @brief Construct with path */
    explicit LandmarkReader(filesystem::path landmarksPath);

    /** @brief Set the path to the Landmarks file */
    void setLandmarksPath(const filesystem::path& path);

    /** @brief Read the Landmarks file */
    void read();

    /** @brief Get the parsed fixed landmarks */
    rt::LandmarkContainer getFixedLandmarks();

    /** @brief Get the parsed moving landmarks */
    rt::LandmarkContainer getMovingLandmarks();

private:
    /** Path to the Landmarks file */
    filesystem::path path_;

    /** Fixed landmarks container */
    LandmarkContainer fixed_;

    /** Moving landmarks container */
    LandmarkContainer moving_;
};

}  // namespace rt