#pragma once

/** @file */

#include <itkSmartPointer.h>
#include <opencv2/core.hpp>

namespace rt
{

/** @brief Convert an itk::Image to a cv::Mat */
template <typename ITKImageType>
auto ITKImageToCVMat(const itk::SmartPointer<ITKImageType>& img) -> cv::Mat;

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
auto CVMatToITKImage(const cv::Mat& img) -> typename ITKImageType::Pointer;
}  // namespace rt

#include "ITKOpenCVBridgeImpl.hpp"