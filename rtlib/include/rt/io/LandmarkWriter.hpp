#pragma once

/** @file */

#include <boost/filesystem.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

#include "rt/LandmarkRegistrationBase.hpp"

namespace rt
{

/**
 * @brief Landmarks file writer
 *
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

namespace graph
{

class LandmarkWriterNode : public smgl::Node
{
    using Path = boost::filesystem::path;
    using Metadata = smgl::Metadata;

public:
    LandmarkWriterNode();
    smgl::InputPort<Path> path{&path_};
    smgl::InputPort<LandmarkContainer> fixed{&fixed_};
    smgl::InputPort<LandmarkContainer> moving{&moving_};

private:
    LandmarkWriter writer_;
    Path path_;
    LandmarkContainer fixed_;
    LandmarkContainer moving_;

    Metadata serialize_(bool /*unused*/, const Path& /*unused*/) override;
    void deserialize_(const Metadata& meta, const Path& /*unused*/) override;
};

}  // namespace graph
}  // namespace rt