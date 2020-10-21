#include "rt/BSplineLandmarkWarping.hpp"

#include <itkLandmarkDisplacementFieldSource.h>

#include "rt/ITKImageTypes.hpp"
#include "rt/types/CompositeTransform.hpp"
#include "rt/util/ITKOpenCVBridge.hpp"

using namespace rt;
namespace rtg = rt::graph;
namespace fs = boost::filesystem;

void BSplineLandmarkWarping::setFixedImage(const cv::Mat& f) { fixedImg_ = f; }

BSplineLandmarkWarping::Transform::Pointer BSplineLandmarkWarping::compute()
{
    // Size checks
    if (fixedImg_.empty() || fixedLdmks_.empty() || movingLdmks_.empty()) {
        throw std::invalid_argument("Empty input parameter");
    }

    // Convert to 8UC3
    auto fixed8u = QuantizeImage(fixedImg_, CV_8U);
    auto fixedImg = CVMatToITKImage<Image8UC3>(fixed8u);

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

rtg::BSplineLandmarkWarpingNode::BSplineLandmarkWarpingNode()
{
    registerInputPort("fixedLandmarks", fixedLandmarks);
    registerInputPort("fixedImage", fixedImage);
    registerInputPort("movingLandmarks", movingLandmarks);
    registerOutputPort("transform", transform);

    compute = [=]() {
        std::cout << "Running B-spline landmark registration..." << std::endl;
        reg_.setFixedLandmarks(fixed_);
        reg_.setFixedImage(fixedImg_);
        reg_.setMovingLandmarks(moving_);
        tfm_ = reg_.compute();
    };
}

smgl::Metadata rtg::BSplineLandmarkWarpingNode::serialize_(
    bool useCache, const Path& cacheDir)
{
    Metadata m;
    if (useCache) {
        WriteTransform(cacheDir / "bspline.tfm", tfm_);
        m["transform"] = "bspline.tfm";
    }

    return m;
}

void rtg::BSplineLandmarkWarpingNode::deserialize_(
    const Metadata& meta, const Path& cacheDir)
{
    if (meta.contains("transform")) {
        auto file = meta["transform"].get<std::string>();
        // TODO: Read affine transform
    }
}