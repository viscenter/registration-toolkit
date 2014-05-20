#include "itkBSplineTransform.h"
#include "itkCastImageFilter.h"
#include "itkCommand.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegistrationMethod.h"
#include "itkIndex.h"
#include "itkKernelTransform.h"
#include "itkLandmarkDisplacementFieldSource.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkMemoryProbesCollectorBase.h"
#include "itkRGBPixel.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkResampleImageFilter.h"
#include "itkSquaredDifferenceImageFilter.h"
#include "itkTimeProbesCollectorBase.h"
#include "itkTransformFileWriter.h"
#include "itkVector.h"
#include "itkVectorLinearInterpolateImageFunction.h"
#include "itkWarpImageFilter.h"
#include "itkWarpVectorImageFilter.h"

#include <fstream>
#include <iostream>
#include <string>

const unsigned int ImageDimension = 2;
const unsigned int SpaceDimension = ImageDimension;
const unsigned int SplineOrder = 3;

typedef double CoordinateRepType;

typedef itk::BSplineTransform< CoordinateRepType,
			       SpaceDimension,
			       SplineOrder >     BSplineTransformType;

typedef BSplineTransformType::ParametersType     BSplineParametersType;

typedef itk::RGBPixel<unsigned char>  ColorPixelType;

typedef itk::Image< ColorPixelType, ImageDimension > ColorImageType;

typedef itk::ResampleImageFilter<
                          ColorImageType,
                          ColorImageType >    ResampleFilterType;

typedef itk::CastImageFilter<
                      ColorImageType,
                      ColorImageType > CastFilterType;

typedef itk::ImageFileReader< ColorImageType >  ColorReaderType;
typedef itk::ImageFileWriter< ColorImageType >  ColorWriterType;

typedef   unsigned char GrayPixelType;

typedef itk::Image<GrayPixelType, ImageDimension> GrayImageType;

typedef itk::ImageRegistrationMethod<
                                    GrayImageType,
                                    GrayImageType >    RegistrationType;

typedef   float          VectorComponentType;

typedef itk::KernelTransform< double, ImageDimension > KernelTransformType;
typedef KernelTransformType::ParametersType KernelParametersType;

typedef   itk::Vector< VectorComponentType, ImageDimension >    VectorType;

typedef   itk::Image< VectorType,  ImageDimension >   DisplacementFieldType;

typedef   itk::ImageFileReader< GrayImageType >  GrayReaderType;
typedef   itk::ImageFileWriter< GrayImageType >  GrayWriterType;

typedef itk::LandmarkDisplacementFieldSource<
                                DisplacementFieldType
                                             >  DisplacementSourceType;

typedef DisplacementSourceType::LandmarkContainerPointer   LandmarkContainerPointer;
typedef DisplacementSourceType::LandmarkContainer          LandmarkContainerType;
typedef DisplacementSourceType::LandmarkPointType          LandmarkPointType;

typedef itk::WarpImageFilter< GrayImageType,
                                GrayImageType,
                                DisplacementFieldType  >  GrayFilterType;

typedef itk::LinearInterpolateImageFunction<
                       GrayImageType, double >  GrayInterpolatorType;

typedef itk::WarpVectorImageFilter< ColorImageType,
                                ColorImageType,
                                DisplacementFieldType  >  ColorFilterType;

typedef itk::VectorLinearInterpolateImageFunction<
                       ColorImageType, double >  ColorInterpolatorType;

typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;

typedef itk::MattesMutualInformationImageToImageMetric<
                                    GrayImageType,
                                    GrayImageType >    MetricType;
