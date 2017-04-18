#include "rt/BSplineLandmarkWarping.hpp"

using namespace rt;

BSplineLandmarkWarping::Transform::Pointer BSplineLandmarkWarping::compute()
{
    // Size checks
    if (fixedImg_.IsNull() || fixedLdmks_.empty() || movingLdmks_.empty()) {
        throw std::runtime_error("Empty input parameter");
    }

    // Setup new transform
    output_ = Transform::New();

    // Initialize transform
    auto landmarkTransformer = TransformInitializer::New();
    landmarkTransformer->SetOutputSpacing(fixedImg_->GetSpacing());
    landmarkTransformer->SetOutputOrigin(fixedImg_->GetOrigin());
    landmarkTransformer->SetOutputRegion(fixedImg_->GetLargestPossibleRegion());
    landmarkTransformer->SetOutputDirection(fixedImg_->GetDirection());

    // Convert landmarks to what deformation field needs
    using BSplineLandmarkContainer = TransformInitializer::LandmarkContainer;
    BSplineLandmarkContainer::Pointer f = BSplineLandmarkContainer::New();
    BSplineLandmarkContainer::Pointer m = BSplineLandmarkContainer::New();
    std::copy(fixedLdmks_.begin(), fixedLdmks_.end(), f->begin());
    std::copy(movingLdmks_.begin(), movingLdmks_.end(), m->begin());

    // Assign landmarks
    landmarkTransformer->SetSourceLandmarks(f);
    landmarkTransformer->SetTargetLandmarks(m);

    // Run registration
    landmarkTransformer->UpdateLargestPossibleRegion();

    // Get output
    output_ = landmarkTransformer->GetKernelTransform();

    return output_;
}