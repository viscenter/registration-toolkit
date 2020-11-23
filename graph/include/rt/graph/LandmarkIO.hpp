#pragma once

/** @file */

#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

#include "rt/filesystem.hpp"
#include "rt/io/LandmarkReader.hpp"
#include "rt/io/LandmarkWriter.hpp"

namespace rt
{
namespace graph
{

/**
 * @brief Landmark File Reader
 * @see LandmarkReader
 */
class LandmarkReaderNode : public smgl::Node
{
public:
    /** Default constructor */
    LandmarkReaderNode();

    /** @name Input Ports */
    /**@{*/
    /** @brief Landmarks file path port */
    smgl::InputPort<filesystem::path> path{&path_};
    /**@}*/

    /** @name Output Ports */
    /**@{*/
    /** @brief Fixed landmarks port */
    smgl::OutputPort<LandmarkContainer> fixedLandmarks{&fixed_};
    /** @brief Moving landmarks port */
    smgl::OutputPort<LandmarkContainer> movingLandmarks{&moving_};
    /**@}*/

private:
    /** Landmark file reader */
    LandmarkReader reader_;
    /** File path */
    filesystem::path path_;
    /** Loaded fixed landmarks */
    LandmarkContainer fixed_;
    /** Loaded moving landmarks */
    LandmarkContainer moving_;
    /** Graph serialize */
    smgl::Metadata serialize_(
        bool /*unused*/, const filesystem::path& /*unused*/) override;
    /** Graph deserialize */
    void deserialize_(
        const smgl::Metadata& meta,
        const filesystem::path& /*unused*/) override;
};

/**
 * @brief Landmark File Writer
 * @see LandmarkWriter
 */
class LandmarkWriterNode : public smgl::Node
{
public:
    /** Default constructor */
    LandmarkWriterNode();

    /** @name Input Ports */
    /**@{*/
    /** @brief Landmarks file path port */
    smgl::InputPort<filesystem::path> path{&path_};
    /** @brief Fixed landmarks port */
    smgl::InputPort<LandmarkContainer> fixed{&fixed_};
    /** @brief Moving landmarks port */
    smgl::InputPort<LandmarkContainer> moving{&moving_};
    /**@}*/

private:
    /** Landmarks writer */
    LandmarkWriter writer_;
    /** File path */
    filesystem::path path_;
    /** Fixed landmarks */
    LandmarkContainer fixed_;
    /** Moving landmarks */
    LandmarkContainer moving_;
    /** Graph serialize */
    smgl::Metadata serialize_(
        bool /*unused*/, const filesystem::path& /*unused*/) override;
    /** Graph deserialize */
    void deserialize_(
        const smgl::Metadata& meta,
        const filesystem::path& /*unused*/) override;
};

}  // namespace graph
}  // namespace rt