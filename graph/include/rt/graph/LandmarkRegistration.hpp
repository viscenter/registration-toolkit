#pragma once

/** @file */

#include <opencv2/core.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

#include "rt/AffineLandmarkRegistration.hpp"
#include "rt/BSplineLandmarkWarping.hpp"
#include "rt/LandmarkDetector.hpp"
#include "rt/filesystem.hpp"
#include "rt/types/Transforms.hpp"

namespace rt::graph
{

/**
 * @brief Automatic landmark detector
 * @see LandmarkDetector
 */
class LandmarkDetectorNode : public smgl::Node
{
public:
    /** Default constructor */
    LandmarkDetectorNode();

    /** @name Input Ports */
    /**@{*/
    /** @brief Fixed image port */
    smgl::InputPort<cv::Mat> fixedImage{&fixedImg_};
    /** @brief Moving image port */
    smgl::InputPort<cv::Mat> movingImage{&movingImg_};
    /** @copydoc LandmarkDetector::setMatchRatio(float) */
    smgl::InputPort<float> matchRatio{
        &detector_, &LandmarkDetector::setMatchRatio};
    /**@}*/

    /** @name Output Ports */
    /**@{*/
    /** @brief Fixed landmarks port */
    smgl::OutputPort<LandmarkContainer> fixedLandmarks{&fixedLdm_};
    /** @brief Moving landmarks port */
    smgl::OutputPort<LandmarkContainer> movingLandmarks{&movingLdm_};
    /**@}*/

private:
    /** Landmark detector */
    LandmarkDetector detector_;
    /** Fixed image */
    cv::Mat fixedImg_;
    /** Moving image */
    cv::Mat movingImg_;
    /** Detected fixed landmarks */
    LandmarkContainer fixedLdm_;
    /** Detected moving landmarks */
    LandmarkContainer movingLdm_;
    /** Graph serialize */
    smgl::Metadata serialize_(
        bool useCache, const filesystem::path& cacheDir) override;
    /** Graph serialize */
    void deserialize_(
        const smgl::Metadata& meta, const filesystem::path& cacheDir) override;
};

/**
 * @brief Affine registration using matching landmarks
 * @see AffineLandmarkRegistration
 */
class AffineLandmarkRegistrationNode : public smgl::Node
{
public:
    /** Default constructor */
    AffineLandmarkRegistrationNode();

    /** @name Input Ports */
    /**@{*/
    /** @brief Fixed landmarks port */
    smgl::InputPort<LandmarkContainer> fixedLandmarks;
    /** @brief Moving landmarks port */
    smgl::InputPort<LandmarkContainer> movingLandmarks;
    /** @copydoc AffineLandmarkRegistration::setReportMetrics(bool) */
    smgl::InputPort<bool> reportMetrics;
    /**@}*/

    /** @name Output Ports */
    /**@{*/
    /** @brief Result transform port */
    smgl::OutputPort<Transform::Pointer> transform;
    /**@}*/

private:
    /** Affine registration */
    AffineLandmarkRegistration reg_;
    /** Fixed image landmarks */
    LandmarkContainer fixed_;
    /** Moving image landmarks */
    LandmarkContainer moving_;
    /** Computed transform */
    Transform::Pointer tfm_;
    /** Graph serialize */
    auto serialize_(bool useCache, const filesystem::path& cacheDir)
        -> smgl::Metadata override;
    /** Graph deserialize */
    void deserialize_(
        const smgl::Metadata& meta, const filesystem::path& cacheDir) override;
};

/**
 * @brief B-spline registration using matching landmarks
 * @see BSplineLandmarkWarping
 */
class BSplineLandmarkWarpingNode : public smgl::Node
{
public:
    /** Default constructor */
    BSplineLandmarkWarpingNode();

    /** @name Input Ports */
    /**@{*/
    /** @brief Fixed landmarks port */
    smgl::InputPort<LandmarkContainer> fixedLandmarks{&fixed_};
    /** @brief Fixed image port */
    smgl::InputPort<cv::Mat> fixedImage{&fixedImg_};
    /** @brief Moving landmarks port */
    smgl::InputPort<LandmarkContainer> movingLandmarks{&moving_};
    /**@}*/

    /** @name Output Ports */
    /**@{*/
    /** @brief Result transform port */
    smgl::OutputPort<Transform::Pointer> transform{&tfm_};
    /**@}*/

private:
    /** BSpline registration */
    BSplineLandmarkWarping reg_;
    /** Fixed image landmarks */
    LandmarkContainer fixed_;
    /** Fixed image */
    cv::Mat fixedImg_;
    /** Moving image landmarks */
    LandmarkContainer moving_;
    /** Computed transform */
    Transform::Pointer tfm_;
    /** Graph serialize */
    smgl::Metadata serialize_(
        bool useCache, const filesystem::path& cacheDir) override;
    /** Graph deserialize */
    void deserialize_(
        const smgl::Metadata& meta, const filesystem::path& cacheDir) override;
};

}  // namespace rt