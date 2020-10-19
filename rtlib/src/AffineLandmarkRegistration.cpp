#include "rt/AffineLandmarkRegistration.hpp"

#include "rt/types/CompositeTransform.hpp"

using namespace rt;
namespace rtg = rt::graph;

AffineLandmarkRegistration::Transform::Pointer
AffineLandmarkRegistration::compute()
{
    using TransformInitializer =
        itk::LandmarkBasedTransformInitializer<Transform, Image8UC3, Image8UC3>;

    // Setup new transform
    output_ = Transform::New();

    // Initialize transform
    auto landmarkTransformInit = TransformInitializer::New();
    landmarkTransformInit->SetFixedLandmarks(fixedLdmks_);
    landmarkTransformInit->SetMovingLandmarks(movingLdmks_);
    output_->SetIdentity();
    landmarkTransformInit->SetTransform(output_);
    landmarkTransformInit->InitializeTransform();

    return output_;
}

AffineLandmarkRegistration::Transform::Pointer
AffineLandmarkRegistration::getTransform()
{
    return output_;
}

rtg::AffineLandmarkRegistrationNode::AffineLandmarkRegistrationNode()
{
    registerInputPort("fixedLandmarks", fixedLandmarks);
    registerInputPort("movingLandmarks", movingLandmarks);
    registerOutputPort("transform", transform);

    compute = [=]() {
        std::cout << "Running affine registration..." << std::endl;
        reg_.setFixedLandmarks(fixed_);
        reg_.setMovingLandmarks(moving_);
        tfm_ = reg_.compute();
    };
}

smgl::Metadata rtg::AffineLandmarkRegistrationNode::serialize_(
    bool useCache, const Path& cacheDir)
{
    Metadata m;
    if (useCache) {
        WriteTransform(cacheDir / "affine.tfm", tfm_);
        m["transform"] = "affine.tfm";
    }

    return m;
}

void rtg::AffineLandmarkRegistrationNode::deserialize_(
    const Metadata& meta, const Path& cacheDir)
{
    if (meta.contains("transform")) {
        auto file = meta["transform"].get<std::string>();
        // TODO: Read affine transform
    }
}