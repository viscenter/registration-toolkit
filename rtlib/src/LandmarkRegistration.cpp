// LandmarkRegistration.cpp
#include "LandmarkRegistration.hpp"

ResampleFilter::Pointer LandmarkRegistration::getAffineTransform() {
    return landmarkTransform_;
}

void LandmarkRegistration::compute_() {
    clear_();
    generate_affine_transform_();
}

void LandmarkRegistration::generate_affine_transform_() {
    // Generate the affine deformTransform
    LandmarkTransformInitializer::Pointer landmarkTransformInit =
    LandmarkTransformInitializer::New();
    landmarkTransformInit->SetFixedLandmarks(fixedLandmarks);
    landmarkTransformInit->SetMovingLandmarks(movingLandmarks);

    landmarkTransform_->SetIdentity();
    landmarkTransformInit->SetTransform(landmarkTransform_);
    landmarkTransformInit->InitializeTransform();
}

void LandmarkRegistration::clear_() {
    landmarkTransform_ = AffineTransform::New();
}