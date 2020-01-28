#pragma once

#include <itkImage.h>
#include <itkImageRegionIterator.h>
#include <opencv2/core.hpp>

#include "rt/ImageTypes.hpp"

namespace rt
{

template <typename ITKImageType>
static cv::Mat ITKImageToCVMat(const ITKImageType* img)
{
    using PixelType = typename ITKImageType::PixelType;
    using ValueType = typename itk::NumericTraits<PixelType>::ValueType;

    cv::Mat out;

    typename ITKImageType::RegionType region = img->GetLargestRegionPossible();
    typename ITKImageType::SizeType size = region.GetSize();

    return out;
}

}  // namespace rt
