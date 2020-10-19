#pragma once

/** @file */

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

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
    LandmarkContainer fixed_;

    /** Moving landmarks container */
    LandmarkContainer moving_;
};

namespace graph
{

class LandmarkReaderNode : public smgl::Node
{
    using Path = boost::filesystem::path;
    using Metadata = smgl::Metadata;

public:
    LandmarkReaderNode();
    smgl::InputPort<Path> path{&path_};
    smgl::OutputPort<LandmarkContainer> fixedLandmarks{&fixed_};
    smgl::OutputPort<LandmarkContainer> movingLandmarks{&moving_};

private:
    LandmarkReader reader_;
    Path path_;
    LandmarkContainer fixed_;
    LandmarkContainer moving_;

    Metadata serialize_(bool /*unused*/, const Path& /*unused*/) override;
    void deserialize_(const Metadata& meta, const Path& /*unused*/) override;
};

}  // namespace graph
}  // namespace rt