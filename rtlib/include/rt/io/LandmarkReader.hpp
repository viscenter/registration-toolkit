#pragma once

/** @file */

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>

#include "rt/LandmarkRegistrationBase.hpp"

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
    explicit LandmarkReader(boost::filesystem::path landmarksPath);

    /** @brief Set the path to the Landmarks file */
    void setLandmarksPath(const boost::filesystem::path& path);

    /** @brief Read the Landmarks file */
    void read();

    /** @brief Get the parsed fixed landmarks */
    rt::LandmarkContainer getFixedLandmarks();

    /** @brief Get the parsed moving landmarks */
    rt::LandmarkContainer getMovingLandmarks();

private:
    /** Path to the Landmarks file */
    boost::filesystem::path path_;

    /** Fixed landmarks container */
    rt::LandmarkContainer fixed_;

    /** Moving landmarks container */
    rt::LandmarkContainer moving_;
};
}  // namespace rt