#pragma once

#include <itkBSplineTransform.h>

#include "rt/ImageTypes.hpp"

namespace rt
{

class DeformableRegistration
{
public:
    static constexpr size_t DEFAULT_ITERATIONS = 100;
    static constexpr double DEFAULT_RELAXATION = 0.85;
    static constexpr double DEFAULT_GRAD_MAG_TOLERANCE = 0.0001;
    using Transform = itk::BSplineTransform<double, 2, 3>;

    DeformableRegistration()
        : iterations_(DEFAULT_ITERATIONS)
        , relaxationFactor_(DEFAULT_RELAXATION)
        , gradientMagnitudeTolerance_(DEFAULT_GRAD_MAG_TOLERANCE)
    {
    }

    void setFixedImage(Image8UC3::Pointer i) { fixedImage_ = i; }
    void setMovingImage(Image8UC3::Pointer i) { movingImage_ = i; }
    void setNumberOfIterations(size_t i) { iterations_ = i; }

    Transform::Pointer compute();

    Transform::Pointer getTransform() { return output_; }
private:
    Image8UC3::Pointer fixedImage_;
    Image8UC3::Pointer movingImage_;

    //// Parameters ////
    // Hard iteration limit
    size_t iterations_;
    // Optimizer step length is reduced by this factor each iteration
    double relaxationFactor_;
    // Stop condition if change in metric is less than this value
    double gradientMagnitudeTolerance_;

    Transform::Pointer output_;
};
}
