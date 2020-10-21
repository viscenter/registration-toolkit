#pragma once

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

#include "rt/DeformableRegistration.hpp"
#include "rt/types/Transforms.hpp"

namespace rt
{
namespace graph
{

class DeformableRegistrationNode : public smgl::Node
{
public:
    DeformableRegistrationNode();

    smgl::InputPort<cv::Mat> fixedImage{
        &reg_, &DeformableRegistration::setFixedImage};
    smgl::InputPort<cv::Mat> movingImage{
        &reg_, &DeformableRegistration::setMovingImage};
    smgl::InputPort<int> iterations{
        &reg_, &DeformableRegistration::setNumberOfIterations};
    smgl::OutputPort<Transform::Pointer> transform{&tfm_};

private:
    DeformableRegistration reg_;
    Transform::Pointer tfm_;

    // TODO: Serialization
};

}  // namespace graph
}  // namespace rt