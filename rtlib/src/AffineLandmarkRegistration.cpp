#include "rt/AffineLandmarkRegistration.hpp"

using namespace rt;

AffineLandmarkRegistration::Transform::Pointer
AffineLandmarkRegistration::compute()
{
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