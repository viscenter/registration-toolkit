#pragma once

#include <itkSmartPointer.h>
#include <opencv2/core.hpp>

namespace rt
{

/** @brief Convert an itk::Image to a cv::Mat */
template <typename ITKImageType>
cv::Mat ITKImageToCVMat(const itk::SmartPointer<ITKImageType>& img);

/**
 * @brief Convert a cv::Mat to an itk::Image
 *
 * @note Uses ColorConvertImage() to match number of channels required by
 * ITKImageType, but does not convert depth.
 *
 * @throws std::invalid_argument if input type has incompatible pixel type or
 * input does not have 1, 3, or 4 channels,
 */
template <typename ITKImageType>
typename ITKImageType::Pointer CVMatToITKImage(const cv::Mat& img);
}  // namespace rt

#include "ITKOpenCVBridgeImpl.hpp"