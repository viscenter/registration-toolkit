#pragma once

#include <boost/filesystem.hpp>

#include "rt/ImageTypes.hpp"
#include "rt/LandmarkRegistrationBase.hpp"

namespace rt
{
/**
 * @class LandmarkReader
 * @author Zack Anderson
 * @brief Landmarks file reader
 *
 * @ingroup landmark
 */
class LandmarkReader
{
public:
    /** @brief Default constructor */
    LandmarkReader() = default;

    explicit LandmarkReader(boost::filesystem::path landmarksPath)
        : landmarksPath_(std::move(landmarksPath))
    {
    }

    /** @brief Set the fixed image */
    void setFixedImage(const Image8UC3::Pointer i) { fixedImage_ = i; }

    /** @brief Set the moving image */
    void setMovingImage(const Image8UC3::Pointer i) { movingImage_ = i; }

    /** @brief Set the path to the Landmarks file */
    void setLandmarksPath(const boost::filesystem::path& path)
    {
        landmarksPath_ = path;
    }

    /** @brief Read the Landmarks file */
    void read();

    void readRaw();

    /** @brief Get the parsed fixed landmarks */
    rt::LandmarkContainer getFixedLandmarks() { return fixedLandmarks_; }

    /** @brief Get the parsed moving landmarks */
    rt::LandmarkContainer getMovingLandmarks() { return movingLandmarks_; }

private:
    /** Path to the Landmarks file */
    boost::filesystem::path landmarksPath_;

    /** Fixed image */
    Image8UC3::Pointer fixedImage_;

    /** Moving image */
    Image8UC3::Pointer movingImage_;

    /** Fixed landmarks container */
    rt::LandmarkContainer fixedLandmarks_;

    /** Moving landmarks container */
    rt::LandmarkContainer movingLandmarks_;
};
}  // namespace rt