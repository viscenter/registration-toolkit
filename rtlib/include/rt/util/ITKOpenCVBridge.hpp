#pragma once

#include <cstring>
#include <exception>
#include <typeinfo>

#include <itkConvertPixelBuffer.h>
#include <itkImage.h>
#include <itkImageRegionIterator.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "rt/ITKImageTypes.hpp"
#include "rt/util/ImageConversion.hpp"

namespace rt
{

template <typename ITKImageType>
cv::Mat ITKImageToCVMat(const itk::SmartPointer<ITKImageType> img)
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
    if (cns == 4) {
        cv::cvtColor(tmp, out, cv::COLOR_RGBA2BGRA);
    } else if (cns == 3) {
        cv::cvtColor(tmp, out, cv::COLOR_RGB2BGR);
    } else {
        tmp.copyTo(out);
    }

    return out;
}

template <typename ITKImageType, typename CVPixelType>
typename ITKImageType::Pointer CVMatToITKImage(const cv::Mat& img)
{
    // Typedefs
    using ITKPixelType = typename ITKImageType::PixelType;
    using ConvertPixelTraits = itk::DefaultConvertPixelTraits<ITKPixelType>;
    using ConvertBuffer =
        itk::ConvertPixelBuffer<CVPixelType, ITKPixelType, ConvertPixelTraits>;

    // Dimensions
    auto w = img.cols;
    auto h = img.rows;
    auto cns = img.channels();

    // We won't convert depth, so make sure depth matches
    if (cns == 4) {
        if (typeid(itk::RGBAPixel<CVPixelType>) != typeid(ITKPixelType)) {
            throw std::invalid_argument("Image depths don't match");
        }
    } else if (cns == 3) {
        if (typeid(itk::RGBPixel<CVPixelType>) != typeid(ITKPixelType)) {
            throw std::invalid_argument("Image depths don't match");
        }
    } else if (cns == 1) {
        if (typeid(CVPixelType) != typeid(ITKPixelType)) {
            throw std::invalid_argument("Image depths don't match");
        }
    } else {
        throw std::invalid_argument(
            "Unsupported channels: " + std::to_string(cns));
    }

    typename ITKImageType::RegionType region;
    typename ITKImageType::RegionType::SizeType size;
    typename ITKImageType::RegionType::IndexType start;
    typename ITKImageType::SpacingType spacing;
    size.Fill(1);
    size[0] = w;
    size[1] = h;
    start.Fill(0);
    spacing.Fill(1);
    region.SetSize(size);
    region.SetIndex(start);

    // BGR -> RGB
    cv::Mat tmp;
    if (img.channels() == 4) {
        cv::cvtColor(img, tmp, cv::COLOR_BGRA2RGBA);
    } else if (img.channels() == 3) {
        cv::cvtColor(img, tmp, cv::COLOR_BGR2RGB);
    } else {
        tmp = img;
    }

    auto out = ITKImageType::New();
    out->SetRegions(region);
    out->SetSpacing(spacing);
    out->Allocate();

    ConvertBuffer::Convert(
        reinterpret_cast<CVPixelType*>(tmp.data), cns,
        out->GetPixelContainer()->GetBufferPointer(),
        out->GetPixelContainer()->Size());

    return out;
}

template <typename ITKImageType>
typename ITKImageType::Pointer CVMatToITKImage(const cv::Mat& img)
{
    // Out channels
    using ITKPixelType = typename ITKImageType::PixelType;
    auto outCns =
        itk::NumericTraits<ITKPixelType>::MeasurementVectorType::Dimension;

    // Color convert
    auto tmp = rt::ColorConvertImage(img, outCns);

    switch (tmp.depth()) {
        case CV_8U:
            return CVMatToITKImage<ITKImageType, uint8_t>(tmp);
        case CV_8S:
            return CVMatToITKImage<ITKImageType, int8_t>(tmp);
        case CV_16U:
            return CVMatToITKImage<ITKImageType, uint16_t>(tmp);
        case CV_16S:
            return CVMatToITKImage<ITKImageType, int16_t>(tmp);
        case CV_32S:
            return CVMatToITKImage<ITKImageType, int32_t>(tmp);
        case CV_32F:
            return CVMatToITKImage<ITKImageType, float>(tmp);
        case CV_64F:
            return CVMatToITKImage<ITKImageType, double>(tmp);
        default:
            throw std::invalid_argument("Image type not supported");
    }
}
}  // namespace rt
