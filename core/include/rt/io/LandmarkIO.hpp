#pragma once

/** @file */

#include <opencv2/core.hpp>
#include "rt/LandmarkRegistrationBase.hpp"
#include "rt/filesystem.hpp"

namespace rt
{

/**
 * @brief Landmarks file writer
 *
 * Used to write matching landmark pairs to a file
 */
class LandmarkWriter
{
public:
    /** @brief Set the output file path */
    void setPath(const filesystem::path& p);

    /**
     * @brief Set the fixed landmarks
     *
     * @note Must match the size of the container provided by
     * setMovingLandmarks()
     */
    void setFixedLandmarks(const LandmarkContainer& f);

    /**
     * @brief Set the moving landmarks
     *
     * @note Must match the size of the container provided by
     * setFixedLandmarks()
     */
    void setMovingLandmarks(const LandmarkContainer& m);

    /**
     * @brief Write the Landmarks file to disk
     *
     * @throws std::runtime_error if path is empty, landmark container sizes
     * don't match, or path cannot be opened
     */
    void write();

private:
    /** Output path */
    filesystem::path path_;
    /** Fixed landmarks */
    LandmarkContainer fixed_;
    /** Moving landmarks */
    LandmarkContainer moving_;
};

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
    LandmarkContainer getFixedLandmarks();

    /** @brief Get the parsed moving landmarks */
    LandmarkContainer getMovingLandmarks();

private:
    /** Path to the Landmarks file */
    filesystem::path path_;

    /** Fixed landmarks container */
    LandmarkContainer fixed_;

    /** Moving landmarks container */
    LandmarkContainer moving_;
};

void WriteLandmarkContainer(
    const filesystem::path& path, const LandmarkContainer& lc);

LandmarkContainer ReadLandmarkContainer(const filesystem::path& path);

}  // namespace rt