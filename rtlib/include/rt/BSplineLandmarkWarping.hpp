#pragma once

/** @file */

#include <boost/filesystem.hpp>
#include <itkBSplineTransform.h>
#include <opencv2/core.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

#include "rt/LandmarkRegistrationBase.hpp"

namespace rt
{
/**
 * @class BSplineLandmarkWarping
 * @brief Generate a B-spline-based transformation that maps an ordered set of
 * landmarks onto a fixed set of landmarks
 */
class BSplineLandmarkWarping : public LandmarkRegistrationBase
{
public:
    /** @brief Transform type returned by this class */
    using Transform = itk::BSplineTransform<double, 2>;

    /**
     * @brief Set the fixed image
     *
     * This must be set in order to compute the transform.
     */
    void setFixedImage(const cv::Mat& f);

    /** @brief Compute the transform */
    Transform::Pointer compute();

    /** @brief Return the computed transform */
    Transform::Pointer getTransform();

private:
    /** Transform */
    Transform::Pointer output_;

    /** Fixed image */
    cv::Mat fixedImg_;
};

namespace graph
{

class BSplineLandmarkWarpingNode : public smgl::Node
{
    using Path = boost::filesystem::path;
    using Metadata = smgl::Metadata;
    using Transform = BSplineLandmarkWarping::Transform;

public:
    BSplineLandmarkWarpingNode();

    smgl::InputPort<LandmarkContainer> fixedLandmarks{&fixed_};
    smgl::InputPort<cv::Mat> fixedImage{fixedImg_};
    smgl::InputPort<LandmarkContainer> movingLandmarks{&moving_};
    smgl::OutputPort<Transform::Pointer> transform{&tfm_};

private:
    BSplineLandmarkWarping reg_;
    LandmarkContainer fixed_;
    cv::Mat fixedImg_;
    LandmarkContainer moving_;
    Transform::Pointer tfm_;

    Metadata serialize_(bool useCache, const Path& cacheDir) override;
    void deserialize_(const Metadata& meta, const Path& cacheDir) override;
};

}  // namespace graph
}