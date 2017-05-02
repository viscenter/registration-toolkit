#include "rt/AffineLandmarkRegistration.hpp"

using namespace rt;

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