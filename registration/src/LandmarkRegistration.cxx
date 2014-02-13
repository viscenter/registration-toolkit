#include "itkVector.h"
#include "itkImage.h"
#include "itkLandmarkDisplacementFieldSource.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkWarpImageFilter.h"
#include "itkWarpVectorImageFilter.h"
#include "itkIndex.h"

#include "itkLinearInterpolateImageFunction.h"
#include "itkVectorLinearInterpolateImageFunction.h"

#include "itkImageRegistrationMethod.h"
#include "itkMattesMutualInformationImageToImageMetric.h"

#include "itkTimeProbesCollectorBase.h"
#include "itkMemoryProbesCollectorBase.h"

#include "itkBSplineTransform.h"
#include "itkRegularStepGradientDescentOptimizer.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkSquaredDifferenceImageFilter.h"

#include "itkRGBPixel.h"

#include <fstream>

//  The following section of code implements a Command observer
//  used to monitor the evolution of the registration process.
#include "itkCommand.h"
class CommandIterationUpdate : public itk::Command
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>   Pointer;
  itkNewMacro( Self );

protected:
  CommandIterationUpdate() {};

public:
  typedef itk::RegularStepGradientDescentOptimizer OptimizerType;
  typedef   const OptimizerType *                  OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
    Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
    OptimizerPointer optimizer =
      dynamic_cast< OptimizerPointer >( object );
    if( !(itk::IterationEvent().CheckEvent( &event )) )
      {
      return;
      }
    std::cout << "Iteration : ";
    std::cout << optimizer->GetCurrentIteration() << "   ";
    std::cout << optimizer->GetValue() << "   ";
    std::cout << std::endl;
    }
};

int main(int argc, char* argv[])
{
	if( argc < 6 )
    {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " landmarksFile fixedImage ";
    std::cerr << "movingImage outputImageFile ";
    std::cerr << "numberOfIterations" << std::endl;
    // TODO allow for output of transformation images to be used in gif
    return EXIT_FAILURE;
    }

  const     unsigned int   ImageDimension = 2;
  typedef   float          VectorComponentType;

  typedef   itk::Vector< VectorComponentType, ImageDimension >    VectorType;

  typedef   itk::Image< VectorType,  ImageDimension >   DisplacementFieldType;


  typedef   unsigned char GrayPixelType;

  typedef itk::Image<GrayPixelType, ImageDimension> GrayImageType;

  typedef   itk::ImageFileReader< GrayImageType >  GrayReaderType;
  typedef   itk::ImageFileWriter< GrayImageType >  GrayWriterType;


  typedef itk::RGBPixel<unsigned char>  ColorPixelType;

  typedef itk::Image< ColorPixelType, ImageDimension > ColorImageType;

  typedef itk::CastImageFilter<
                        ColorImageType,
                        ColorImageType > CastFilterType;

  typedef itk::ImageFileReader< ColorImageType >  ColorReaderType;
  typedef itk::ImageFileWriter< ColorImageType >  ColorWriterType;


  GrayReaderType::Pointer grayFixedReader = GrayReaderType::New();
  grayFixedReader->SetFileName( argv[2] );

  try
    {
    grayFixedReader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  GrayReaderType::Pointer grayMovingReader = GrayReaderType::New();
  GrayWriterType::Pointer grayMovingWriter = GrayWriterType::New();

  grayMovingReader->SetFileName( argv[3] );
  grayMovingWriter->SetFileName( argv[4] );

  try
    {
    grayMovingReader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  GrayImageType::ConstPointer grayFixedImage = grayFixedReader->GetOutput();
  GrayImageType::ConstPointer grayMovingImage = grayMovingReader->GetOutput();

  ColorReaderType::Pointer colorMovingReader = ColorReaderType::New();
  ColorWriterType::Pointer colorMovingWriter = ColorWriterType::New();

  colorMovingReader->SetFileName( argv[3] );
  colorMovingWriter->SetFileName( argv[4] );

  try
    {
    colorMovingReader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  ColorImageType::ConstPointer colorMovingImage = colorMovingReader->GetOutput();

  typedef itk::LandmarkDisplacementFieldSource<
                                DisplacementFieldType
                                             >  DisplacementSourceType;

  DisplacementSourceType::Pointer deformer = DisplacementSourceType::New();

  deformer->SetOutputSpacing( grayFixedImage->GetSpacing() );
  deformer->SetOutputOrigin(  grayFixedImage->GetOrigin() );
  deformer->SetOutputRegion(  grayFixedImage->GetLargestPossibleRegion() );
  deformer->SetOutputDirection( grayFixedImage->GetDirection() );

  //  Create source and target landmarks.
  //
  typedef DisplacementSourceType::LandmarkContainerPointer   LandmarkContainerPointer;
  typedef DisplacementSourceType::LandmarkContainer          LandmarkContainerType;
  typedef DisplacementSourceType::LandmarkPointType          LandmarkPointType;

  LandmarkContainerType::Pointer sourceLandmarks = LandmarkContainerType::New();
  LandmarkContainerType::Pointer targetLandmarks = LandmarkContainerType::New();

  GrayImageType::IndexType sourceIndex, targetIndex;

  LandmarkPointType sourcePoint, targetPoint;

  std::ifstream pointsFile;
  pointsFile.open( argv[1] );

  unsigned int pointId = 0;

  std::cout << std::endl << "Starting Landmark Warping" << std::endl;

  unsigned int sourceX, sourceY, targetX, targetY;

  pointsFile >> sourceX >> sourceY >> targetX >> targetY;

  sourceIndex[0] = sourceX;
  sourceIndex[1] = sourceY;
  targetIndex[0] = targetX;
  targetIndex[1] = targetY;

  while( !pointsFile.fail() )
    {
    grayFixedImage->TransformIndexToPhysicalPoint(sourceIndex, sourcePoint);
    grayMovingImage->TransformIndexToPhysicalPoint(targetIndex, targetPoint);

    // Print the physical points to stdout after
    // reading in pixel indices
    // std::cout << sourcePoint << targetPoint << std::endl;

    sourceLandmarks->InsertElement( pointId, sourcePoint );
    targetLandmarks->InsertElement( pointId, targetPoint );
    pointId++;

    pointsFile >> sourceX >> sourceY >> targetX >> targetY;

    sourceIndex[0] = sourceX;
    sourceIndex[1] = sourceY;
    targetIndex[0] = targetX;
    targetIndex[1] = targetY;
    }

  pointsFile.close();

  deformer->SetSourceLandmarks( sourceLandmarks.GetPointer() );
  deformer->SetTargetLandmarks( targetLandmarks.GetPointer() );

  try
    {
    deformer->UpdateLargestPossibleRegion();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  DisplacementFieldType::ConstPointer displacementField = deformer->GetOutput();

  typedef itk::WarpImageFilter< GrayImageType,
                                GrayImageType,
                                DisplacementFieldType  >  GrayFilterType;

  GrayFilterType::Pointer grayWarper = GrayFilterType::New();

  typedef itk::LinearInterpolateImageFunction<
                       GrayImageType, double >  GrayInterpolatorType;

  GrayInterpolatorType::Pointer grayInterpolator = GrayInterpolatorType::New();

  grayWarper->SetInterpolator( grayInterpolator );


  grayWarper->SetOutputSpacing( displacementField->GetSpacing() );
  grayWarper->SetOutputOrigin(  displacementField->GetOrigin() );

  grayWarper->SetDisplacementField( displacementField );

  grayWarper->SetInput( grayMovingReader->GetOutput() );

  grayMovingWriter->SetInput( grayWarper->GetOutput() );

  try
    {
    grayMovingWriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::WarpVectorImageFilter< ColorImageType,
                                ColorImageType,
                                DisplacementFieldType  >  ColorFilterType;

  ColorFilterType::Pointer colorWarper = ColorFilterType::New();

  typedef itk::VectorLinearInterpolateImageFunction<
                       ColorImageType, double >  ColorInterpolatorType;

  ColorInterpolatorType::Pointer colorInterpolator = ColorInterpolatorType::New();

  colorWarper->SetInterpolator( colorInterpolator );

  // necessary to avoid "input images do not occupy the same physical space" error
  // not necessary in gray image for some reason
  // with color images this gives the desired output colored image
  // TODO make it just use one warper and interpolator etc
  colorWarper->SetCoordinateTolerance(10);

  colorWarper->SetOutputSpacing( displacementField->GetSpacing() );
  colorWarper->SetOutputOrigin(  displacementField->GetOrigin() );

  colorWarper->SetDisplacementField( displacementField );

  colorWarper->SetInput( colorMovingReader->GetOutput() );

  colorMovingWriter->SetInput( colorWarper->GetOutput() );

  try
    {
    colorMovingWriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }
}