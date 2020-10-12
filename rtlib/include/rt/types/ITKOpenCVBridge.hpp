#pragma once

#include <exception>

#include <itkImage.h>
#include <itkImageRegionIterator.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "rt/ImageTypes.hpp"

namespace rt
{

template <typename ITKImageType>
cv::Mat ITKImageToCVMat(const ITKImageType* img)
{

    using PixelType = typename ITKImageType::PixelType;
    using ValueType = typename itk::NumericTraits<PixelType>::ValueType;

    // Make sure the image is not null
    if (!img) {
        throw std::invalid_argument("img is nullptr");
    }

    // Get the channels and dimensions
    auto region = img->GetLargestPossibleRegion();
    auto size = region.GetSize();
    auto cns = itk::NumericTraits<PixelType>::MeasurementVectorType::Dimension;
    auto w = static_cast<uint32_t>(size[0]);
    auto h = static_cast<uint32_t>(size[1]);

    // Get the pixel type depth
    int type{-1};
    if (typeid(ValueType) == typeid(uint8_t)) {
        type = CV_8UC(cns);
    } else if (typeid(ValueType) == typeid(int8_t)) {
        type = CV_8SC(cns);
    } else if (typeid(ValueType) == typeid(uint16_t)) {
        type = CV_16UC(cns);
    } else if (typeid(ValueType) == typeid(int16_t)) {
        type = CV_16SC(cns);
    } else if (typeid(ValueType) == typeid(float)) {
        type = CV_32FC(cns);
    } else if (typeid(ValueType) == typeid(int32_t)) {
        type = CV_32SC(cns);
    } else if (typeid(ValueType) == typeid(double)) {
        type = CV_64FC(cns);
    } else {
        throw std::invalid_argument("Unrecognized pixel type");
    }
    auto tmp = cv::Mat(
        h, w, type, reinterpret_cast<uint8_t*>(img->GetBufferPointer()));

    // RGB -> BGR if needed
    cv::Mat out;
    if (cns == 3) {
        cv::cvtColor(tmp, out, cv::COLOR_RGB2BGR);
    } else {
        tmp.copyTo(out);
    }

    return out;
}

template <typename ITKImageType>
typename ITKImageType::Pointer CVMatToITKImage(const cv::Mat& img)
{
    typename ITKImageType::RegionType region;
    typename ITKImageType::RegionType::SizeType size;
    typename ITKImageType::RegionType::IndexType start;
    typename ITKImageType::SpacingType spacing;

    size.Fill(1);
    size[0] = img.cols;
    size[1] = img.rows;
    start.Fill(0);
    spacing.Fill(1);
    region.SetSize(size);
    region.SetIndex(start);

    // BGR -> RGB
    cv::Mat tmp;
    if(img.channels() == 3) {
        cv::cvtColor(img, tmp, cv::COLOR_BGR2RGB);
    } else if (img.channels() == 4) {
        cv::cvtColor(img, tmp, cv::COLOR_BGRA2RGBA);
    } else {
        cv::copyTo(img, tmp);
    }

    auto out = ITKImageType::New();
    out->SetRegions(region);
    out->SetSpacing(spacing);
    out->Allocate();
}
}  // namespace rt
