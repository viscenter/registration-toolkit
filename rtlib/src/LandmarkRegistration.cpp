// LandmarkRegistration.cpp
#include "LandmarkRegistration.hpp"

ResampleFilter::Pointer LandmarkRegistration::getTransform() {
    return generate_affine_transform_();
}

//TODO: What to return here?
 LandmarkRegistration::generate_affine_transform_() {
    // Generate the affine deformTransform
    io_.computeLandmarks();

    LandmarkTransformInitializer::Pointer landmarkTransformInit =
        LandmarkTransformInitializer::New();
    landmarkTransformInit->SetFixedLandmarks(io_.getFixedLandmarks());
    landmarkTransformInit->SetMovingLandmarks(io_.getMovingLandmarks());

    AffineTransform::Pointer landmark = AffineTransform::New();
    landmarkTransform->SetIdentity();
    landmarkTransformInit->SetTransform(landmarkTransform);
    landmarkTransformInit->InitializeTransform();
    return landmarkTransformInit;
}