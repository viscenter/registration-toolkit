#pragma once

/** @file */

#include <opencv2/core.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

#include "rt/LandmarkRegistrationBase.hpp"
#include "rt/filesystem.hpp"
#include "rt/types/Transforms.hpp"

namespace rt
{
namespace graph
{

/**
 * @brief Create a composite transform from two transforms
 *
 * Concatenates two transforms via composition:
 * \f$T_{1} \circ T_{2} = T_{result}\f$.
 */
class CompositeTransformNode : public smgl::Node
{
public:
    /** Default constructor */
    CompositeTransformNode();

    /** @name Input Ports */
    /**@{*/
    /** @brief First input transform port */
    smgl::InputPort<Transform::Pointer> first{&first_};
    /** @brief Second input transform port */
    smgl::InputPort<Transform::Pointer> second{&second_};
    /**@}*/

    /** @name Output Ports */
    /**@{*/
    /** @brief Composited transform port */
    smgl::OutputPort<Transform::Pointer> result{&result_};
    /**@}*/

private:
    /** First transform */
    Transform::Pointer first_;
    /** Second transform */
    Transform::Pointer second_;
    /** Result transform */
    Transform::Pointer result_;
    /** Graph serialize */
    smgl::Metadata serialize_(
        bool useCache, const filesystem::path& cacheDir) override;
    /** Graph deserialize */
    void deserialize_(
        const smgl::Metadata& meta, const filesystem::path& cacheDir) override;
};

/**
 * @brief Transform File Writer
 * @see WriteTransform
 */
class WriteTransformNode : public smgl::Node
{
public:
    /** Default constructor */
    WriteTransformNode();

    /** @name Input Ports */
    /**@{*/
    /** @brief Transform file path port */
    smgl::InputPort<filesystem::path> path{&path_};
    /** @brief Transform port */
    smgl::InputPort<Transform::Pointer> transform{&tfm_};
    /**@}*/

private:
    /** File path */
    filesystem::path path_;
    /** Transform to write */
    Transform::Pointer tfm_;
    /** Graph serialize */
    smgl::Metadata serialize_(
        bool useCache, const filesystem::path& cacheDir) override;
    /** Graph deserialize */
    void deserialize_(
        const smgl::Metadata& meta, const filesystem::path& cacheDir) override;
};

/**
 * @brief Apply transform to landmarks
 *
 * For every landmark in the container, computes:
 * \f$l_{out} = T^{-1}(l_{in})\f$. This relies upon the assumption (inherited
 * from ITK) that the transform maps from the output space to the input space.
 */
class TransformLandmarksNode : public smgl::Node
{
public:
    /** Default constructor */
    TransformLandmarksNode();

    /** @name Input Ports */
    /**@{*/
    /** @brief Transform port */
    smgl::InputPort<Transform::Pointer> transform{&tfm_};
    /** @brief Input landmarks port */
    smgl::InputPort<LandmarkContainer> landmarksIn{&ldmIn_};
    /**@}*/

    /** @name Output Ports */
    /**@{*/
    /** @brief Output landmarks port */
    smgl::OutputPort<LandmarkContainer> landmarksOut{&ldmOut_};
    /**@}*/

private:
    /** Transform */
    Transform::Pointer tfm_;
    /** Input landmarks */
    LandmarkContainer ldmIn_;
    /** Output landmarks */
    LandmarkContainer ldmOut_;
    /** Graph serialize */
    smgl::Metadata serialize_(
        bool useCache, const filesystem::path& cacheDir) override;
    /** Graph deserialize */
    void deserialize_(
        const smgl::Metadata& meta, const filesystem::path& cacheDir) override;
};

/**
 * @brief Resample an image using a transform
 *
 * Creates a new image the same size as the provided fixed image, then uses
 * the provided transform to map the moving image into this new image space.
 *
 * @see ImageTransformResampler
 */
class ImageResampleNode : public smgl::Node
{
public:
    /** Default constructor */
    ImageResampleNode();

    /** @name Input Ports */
    /**@{*/
    /** @brief Fixed image port */
    smgl::InputPort<cv::Mat> fixedImage{&fixed_};
    /** @brief Moving image port */
    smgl::InputPort<cv::Mat> movingImage{&moving_};
    /** @brief Transform port */
    smgl::InputPort<Transform::Pointer> transform{&tfm_};
    /**
     * @brief Force alpha channel port
     *
     * If true, an alpha channel will be added to the output image even if the
     * moving image does not have one.
     */
    smgl::InputPort<bool> forceAlpha{&forceAlpha_};
    /**@}*/

    /** @name Output Ports */
    /**@{*/
    /** @brief Resampled image port */
    smgl::OutputPort<cv::Mat> resampledImage{&resampled_};
    /**@}*/

private:
    /** Force alpha flag */
    bool forceAlpha_{false};
    /** Fixed image */
    cv::Mat fixed_;
    /** Moving image */
    cv::Mat moving_;
    /** Transform */
    Transform::Pointer tfm_;
    /** Resampled image */
    cv::Mat resampled_;
    /** Graph serialize */
    smgl::Metadata serialize_(
        bool useCache, const filesystem::path& cacheDir) override;
    /** Graph deserialize */
    void deserialize_(
        const smgl::Metadata& meta, const filesystem::path& cacheDir) override;
};

}  // namespace graph
}  // namespace rt