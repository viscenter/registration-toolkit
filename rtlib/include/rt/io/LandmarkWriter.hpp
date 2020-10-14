#pragma once

#include <boost/filesystem.hpp>

#include "rt/LandmarkRegistrationBase.hpp"

namespace rt
{

/**
 * @brief Landmarks file writer
 *
 * @ingroup landmark
 */
class LandmarkWriter
{
public:
    /** @brief Set the output file path */
    void setPath(const boost::filesystem::path& p);

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
    boost::filesystem::path path_;
    /** Fixed landmarks */
    LandmarkContainer fixed_;
    /** Moving landmarks */
    LandmarkContainer moving_;
};
}  // namespace rt