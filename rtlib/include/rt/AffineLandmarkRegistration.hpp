#pragma once

/** @file */

#include <boost/filesystem.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

#include "rt/ITKImageTypes.hpp"
#include "rt/LandmarkRegistrationBase.hpp"

namespace rt
{
/**
 * @class AffineLandmarkRegistration
 * @brief Generate an affine transformation that maps an ordered set of
 * landmarks onto a fixed set of landmarks
 */
class AffineLandmarkRegistration : public LandmarkRegistrationBase
{
public:
    /** @brief Transform type returned by this class */
    using Transform = itk::AffineTransform<double, 2>;

    /** @brief Compute the transform */
    Transform::Pointer compute();

    /** @brief Return the computed transform */
    Transform::Pointer getTransform();

private:
    /** Transform */
    Transform::Pointer output_;
};

namespace graph
{

class AffineLandmarkRegistrationNode : public smgl::Node
{
    using Path = boost::filesystem::path;
    using Metadata = smgl::Metadata;
    using Transform = AffineLandmarkRegistration::Transform;

public:
    AffineLandmarkRegistrationNode();

    smgl::InputPort<LandmarkContainer> fixedLandmarks{&fixed_};
    smgl::InputPort<LandmarkContainer> movingLandmarks{&moving_};
    smgl::OutputPort<Transform::Pointer> transform{&tfm_};

private:
    AffineLandmarkRegistration reg_;
    LandmarkContainer fixed_;
    LandmarkContainer moving_;
    Transform::Pointer tfm_;

    Metadata serialize_(bool useCache, const Path& cacheDir) override;
    void deserialize_(const Metadata& meta, const Path& cacheDir) override;
};

}  // namespace graph
}