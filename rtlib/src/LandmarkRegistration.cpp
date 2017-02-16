// LandmarkRegistration.cpp
#include "LandmarkRegistration.hpp"

ResampleFilter::Pointer LandmarkRegistration::resampleImages() {
    read_points_file_();
    generate_affine_transform_();
    return resample_(); 
}

void LandmarkRegistration::read_points_file_() {
    Landmark fixedPoint, movingPoint;
    Image::IndexType fixedIndex, movingIndex;
    size_t fixedX, fixedY, movingX, movingY;

    while (!pointsFile_.fail()) {
        pointsFile_ >> fixedX >> fixedY >> movingX >> movingY;

        fixedIndex[0] = fixedX;
        fixedIndex[1] = fixedY;
        movingIndex[0] = movingX;
        movingIndex[1] = movingY;

        // Transform landmarks in case spacing still gets used
        fixedImage_->TransformIndexToPhysicalPoint(fixedIndex, fixedPoint);
        movingImage_->TransformIndexToPhysicalPoint(movingIndex, movingPoint);

        fixedLandmarks_.push_back(fixedPoint);
        movingLandmarks_.push_back(movingPoint);
    }
    pointsFile_.close();
}

void LandmarkRegistration::generate_affine_transform_() {
    // Generate the affine deformTransform
    LandmarkTransformInitializer::Pointer landmarkTransformInit =
        LandmarkTransformInitializer::New();
    landmarkTransformInit->SetFixedLandmarks(fixedLandmarks);
    landmarkTransformInit->SetMovingLandmarks(movingLandmarks);

    AffineTransform::Pointer landmark = AffineTransform::New();
    landmarkTransform_->SetIdentity();
    landmarkTransformInit->SetTransform(landmarkTransform_);
    landmarkTransformInit->InitializeTransform();
}

ResampleFilter::Pointer LandmarkRegistration::resample_() {
    // Apply the landmark transform to the image
    ResampleFilter::Pointer resample = ResampleFilter::New();
    ColorInterpolator::Pointer interpolator = ColorInterpolator::New();
    resample->SetInput(movingImage_);
    resample->SetTransform(landmarkTransform_);
    resample->SetInterpolator(interpolator);
    resample->SetSize(fixedImage_->GetLargestPossibleRegion().GetSize());
    resample->SetOutputOrigin(fixedImage_->GetOrigin());
    resample->SetOutputSpacing(fixedImage_->GetSpacing());
    resample->SetOutputDirection(fixedImage_->GetDirection());
    resample->SetDefaultPixelValue(EmptyPixel);
    resample->GetOutput();
    return resample;
}
