#include "rt/BSplineLandmarkWarping.hpp"

using namespace rt;

BSplineLandmarkWarping::Transform::Pointer BSplineLandmarkWarping::compute()
{
    // Size checks
    if (fixedImg_.IsNull() || fixedLdmks_.empty() || movingLdmks_.empty()) {
        throw std::runtime_error("Empty input parameter");
    }

    using TransformInitializer =
        itk::LandmarkBasedTransformInitializer<Transform, Image8UC3, Image8UC3>;

    // Setup new transform
    output_ = Transform::New();
    output_->SetIdentity();

    // Initialize transform
    auto landmarkTransformInit = TransformInitializer::New();
    landmarkTransformInit->SetFixedLandmarks(fixedLdmks_);
    landmarkTransformInit->SetMovingLandmarks(movingLdmks_);
    landmarkTransformInit->SetReferenceImage(fixedImg_);
    landmarkTransformInit->SetTransform(output_);
    landmarkTransformInit->InitializeTransform();

    return output_;
}

BSplineLandmarkWarping::Transform::Pointer
BSplineLandmarkWarping::getTransform()
{
    return output_;
}