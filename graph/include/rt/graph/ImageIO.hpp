#pragma once

/** @file */

#include <opencv2/core.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

#include "rt/filesystem.hpp"

namespace rt::graph
{

/**
 * @brief Image File Reader
 * @see ReadImage
 */
class ImageReadNode : public smgl::Node
{
public:
    /** Default constructor */
    ImageReadNode();

    /** @name Input Ports */
    /**@{*/
    /** @brief Image path port */
    smgl::InputPort<filesystem::path> path{&path_};
    /**@}*/

    /** @name Output Ports */
    /**@{*/
    /** @brief Read image port */
    smgl::OutputPort<cv::Mat> image{&img_};
    /**@}*/

private:
    /** File path */
    filesystem::path path_;
    /** Loaded image */
    cv::Mat img_;
    /** Graph serialize */
    smgl::Metadata serialize_(
        bool /*unused*/, const filesystem::path& /*unused*/) override;
    /** Graph deserialize */
    void deserialize_(
        const smgl::Metadata& meta,
        const filesystem::path& /*unused*/) override;
};

/**
 * @brief Image File Writer
 * @see WriteImage
 */
class ImageWriteNode : public smgl::Node
{
public:
    /** Default constructor */
    ImageWriteNode();

    /** @name Input Ports */
    /**@{*/
    /** @brief Image path port */
    smgl::InputPort<filesystem::path> path{&path_};
    /** @brief Image port */
    smgl::InputPort<cv::Mat> image{&img_};
    /**@}*/

private:
    /** File path */
    filesystem::path path_;
    /** Image to write */
    cv::Mat img_;
    /** Graph serialize */
    smgl::Metadata serialize_(
        bool /*unused*/, const filesystem::path& /*unused*/) override;
    /** Graph deserialize */
    void deserialize_(
        const smgl::Metadata& meta,
        const filesystem::path& /*unused*/) override;
};

}  // namespace rt::graph