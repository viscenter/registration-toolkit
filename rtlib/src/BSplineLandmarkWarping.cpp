#include "rt/BSplineLandmarkWarping.hpp"

#include <itkLandmarkDisplacementFieldSource.h>

#include "rt/ITKImageTypes.hpp"
#include "rt/util/ITKOpenCVBridge.hpp"

using namespace rt;

void BSplineLandmarkWarping::setFixedImage(const cv::Mat& f) { fixedImg_ = f; }

BSplineLandmarkWarping::Transform::Pointer BSplineLandmarkWarping::compute()
{
    auto fixedImg = CVMatToITKImage<Image8UC3>(fixedImg_);

    // Size checks
    if (fixedImg_.empty() || fixedLdmks_.empty() || movingLdmks_.empty()) {
        throw std::invalid_argument("Empty input parameter");
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
    landmarkTransformInit->SetReferenceImage(fixedImg);
    landmarkTransformInit->SetTransform(output_);
    landmarkTransformInit->InitializeTransform();

    return output_;
}

BSplineLandmarkWarping::Transform::Pointer
BSplineLandmarkWarping::getTransform()
{
    return output_;
}
