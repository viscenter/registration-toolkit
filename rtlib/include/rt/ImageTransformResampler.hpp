#pragma once

#include <itkCompositeTransform.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkOpenCVImageBridge.h>
#include <itkResampleImageFilter.h>
#include <opencv2/core.hpp>

#include "rt/ImageTypes.hpp"

namespace rt
{

/**
 * @brief Resample a moving image using a pre-generated transform. Output image
 * is of size s.
 *
 * @ingroup rtlib
 */
template <typename TImageType>
typename TImageType::Pointer ImageTransformResampler(
    const typename TImageType::Pointer& m,
    typename TImageType::SizeType s,
    itk::CompositeTransform<double, 2>::Pointer transform)
{
    using I = itk::NearestNeighborInterpolateImageFunction<TImageType, double>;
    using R = itk::ResampleImageFilter<TImageType, TImageType, double>;

    auto interpolator = I::New();
    auto resample = R::New();
    resample->SetInput(m);
    resample->SetTransform(transform);
    resample->SetInterpolator(interpolator);
    resample->SetSize(s);
    resample->Update();

    return resample->GetOutput();
}

/**
 * @brief Resample a moving image using a pre-generated transform. Output image
 * is of size s.
 *
 * @ingroup rtlib
 */
cv::Mat ImageTransformResampler(
    const cv::Mat& m,
    const cv::Size& s,
    itk::CompositeTransform<double, 2>::Pointer transform);
}