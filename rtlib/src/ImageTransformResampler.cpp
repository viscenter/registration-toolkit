#include "rt/ImageTransformResampler.hpp"

using namespace rt;

using Transform = itk::CompositeTransform<double, 2>::Pointer;

cv::Mat rt::ImageTransformResampler(
    const cv::Mat& m, cv::Size& s, Transform transform)
{
    switch (m.type()) {
        case CV_8UC1: {
            using T = Image8UC1;
            auto i = itk::OpenCVImageBridge::CVMatToITKImage<T>(m);
            i = ImageTransformResampler<T>(i, {s.width, s.height}, transform);
            return itk::OpenCVImageBridge::ITKImageToCVMat<T>(i);
        }
        case CV_8UC3: {
            using T = Image8UC3;
            auto i = itk::OpenCVImageBridge::CVMatToITKImage<T>(m);
            i = ImageTransformResampler<T>(i, {s.width, s.height}, transform);
            return itk::OpenCVImageBridge::ITKImageToCVMat<T>(i);
        }
        case CV_16UC1: {
            using T = Image16UC1;
            auto i = itk::OpenCVImageBridge::CVMatToITKImage<T>(m);
            i = ImageTransformResampler<T>(i, {s.width, s.height}, transform);
            return itk::OpenCVImageBridge::ITKImageToCVMat<T>(i);
        }
        case CV_16UC3: {
            using T = Image16UC3;
            auto i = itk::OpenCVImageBridge::CVMatToITKImage<T>(m);
            i = ImageTransformResampler<T>(i, {s.width, s.height}, transform);
            return itk::OpenCVImageBridge::ITKImageToCVMat<T>(i);
        }
        case CV_32FC1: {
            using T = Image32FC1;
            auto i = itk::OpenCVImageBridge::CVMatToITKImage<T>(m);
            i = ImageTransformResampler<T>(i, {s.width, s.height}, transform);
            return itk::OpenCVImageBridge::ITKImageToCVMat<T>(i);
        }
        case CV_32FC3: {
            using T = Image32FC3;
            auto i = itk::OpenCVImageBridge::CVMatToITKImage<T>(m);
            i = ImageTransformResampler<T>(i, {s.width, s.height}, transform);
            return itk::OpenCVImageBridge::ITKImageToCVMat<T>(i);
        }
        default:
            throw std::runtime_error("unsupported image type");
    }
}
