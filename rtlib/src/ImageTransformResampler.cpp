#include "rt/ImageTransformResampler.hpp"

#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>

#include "rt/ITKImageTypes.hpp"
#include "rt/util/ITKOpenCVBridge.hpp"

using namespace rt;

using Transform = itk::CompositeTransform<double, 2>::Pointer;

template <typename TImageType>
typename TImageType::Pointer InterpolateImage(
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

cv::Mat rt::ImageTransformResampler(
    const cv::Mat& m, const cv::Size& s, Transform transform)
{
    switch (m.type()) {
        case CV_8UC1: {
            using T = Image8UC1;
            auto i = CVMatToITKImage<T>(m);
            i = InterpolateImage<T>(i, {s.width, s.height}, transform);
            return ITKImageToCVMat<T>(i);
        }
        case CV_8UC3: {
            using T = Image8UC3;
            auto i = CVMatToITKImage<T>(m);
            i = InterpolateImage<T>(i, {s.width, s.height}, transform);
            return ITKImageToCVMat<T>(i);
        }
        case CV_8UC2:
        case CV_8UC4: {
            // Extract and transform each channel
            std::vector<cv::Mat> cns;
            cv::split(m, cns);
            for (auto& c : cns) {
                using T = Image8UC1;
                auto i = CVMatToITKImage<T>(c);
                i = InterpolateImage<T>(i, {s.width, s.height}, transform);
                c = ITKImageToCVMat<T>(i);
            }

            cv::Mat result(m.rows, m.cols, CV_8U);
            cv::merge(cns, result);
            return result;
        }
        case CV_16UC1: {
            using T = Image16UC1;
            auto i = CVMatToITKImage<T>(m);
            i = InterpolateImage<T>(i, {s.width, s.height}, transform);
            return ITKImageToCVMat<T>(i);
        }
        case CV_16UC3: {
            using T = Image16UC3;
            auto i = CVMatToITKImage<T>(m);
            i = InterpolateImage<T>(i, {s.width, s.height}, transform);
            return ITKImageToCVMat<T>(i);
        }
        case CV_16UC2:
        case CV_16UC4: {
            // Extract and transform each channel
            std::vector<cv::Mat> cns;
            cv::split(m, cns);
            for (auto& c : cns) {
                using T = Image16UC1;
                auto i = CVMatToITKImage<T>(c);
                i = InterpolateImage<T>(i, {s.width, s.height}, transform);
                c = ITKImageToCVMat<T>(i);
            }

            cv::Mat result(m.rows, m.cols, CV_16U);
            cv::merge(cns, result);
            return result;
        }
        case CV_32FC1: {
            using T = Image32FC1;
            auto i = CVMatToITKImage<T>(m);
            i = InterpolateImage<T>(i, {s.width, s.height}, transform);
            return ITKImageToCVMat<T>(i);
        }
        case CV_32FC3: {
            using T = Image32FC3;
            auto i = CVMatToITKImage<T>(m);
            i = InterpolateImage<T>(i, {s.width, s.height}, transform);
            return ITKImageToCVMat<T>(i);
        }
        case CV_32FC2:
        case CV_32FC4: {
            // Extract and transform each channel
            std::vector<cv::Mat> cns;
            cv::split(m, cns);
            for (auto& c : cns) {
                using T = Image32FC1;
                auto i = CVMatToITKImage<T>(c);
                i = InterpolateImage<T>(i, {s.width, s.height}, transform);
                c = ITKImageToCVMat<T>(i);
            }

            cv::Mat result(m.rows, m.cols, CV_32F);
            cv::merge(cns, result);
            return result;
        }
        default:
            throw std::runtime_error("unsupported image type");
    }
}
