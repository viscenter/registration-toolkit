#include "itkVector.h"
#include "itkImage.h"
#include "itkLandmarkDisplacementFieldSource.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkWarpImageFilter.h"
#include "itkIndex.h"

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


  typedef   unsigned char                            PixelType;
  typedef   itk::Image< PixelType, ImageDimension >       FixedImageType;
  typedef   itk::Image< PixelType, ImageDimension >       MovingImageType;

  typedef   itk::ImageFileReader< FixedImageType  >  FixedReaderType;
  typedef   itk::ImageFileReader< MovingImageType >  MovingReaderType;

  typedef   itk::ImageFileWriter< MovingImageType >  MovingWriterType;

  typedef itk::Image<PixelType, ImageDimension> ImageType;


  FixedReaderType::Pointer fixedReader = FixedReaderType::New();
  fixedReader->SetFileName( argv[2] );

  try
    {
    fixedReader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }


  MovingReaderType::Pointer movingReader = MovingReaderType::New();
  MovingWriterType::Pointer movingWriter = MovingWriterType::New();

  movingReader->SetFileName( argv[3] );
  movingWriter->SetFileName( argv[4] );

  try
    {
    movingReader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  FixedImageType::ConstPointer fixedImage = fixedReader->GetOutput();
  MovingImageType::ConstPointer movingImage = movingReader->GetOutput();


  typedef itk::LandmarkDisplacementFieldSource<
                                DisplacementFieldType
                                             >  DisplacementSourceType;

  DisplacementSourceType::Pointer deformer = DisplacementSourceType::New();

  deformer->SetOutputSpacing( fixedImage->GetSpacing() );
  deformer->SetOutputOrigin(  fixedImage->GetOrigin() );
  deformer->SetOutputRegion(  fixedImage->GetLargestPossibleRegion() );
  deformer->SetOutputDirection( fixedImage->GetDirection() );

  //  Create source and target landmarks.
  //
  typedef DisplacementSourceType::LandmarkContainerPointer   LandmarkContainerPointer;
  typedef DisplacementSourceType::LandmarkContainer          LandmarkContainerType;
  typedef DisplacementSourceType::LandmarkPointType          LandmarkPointType;

  LandmarkContainerType::Pointer sourceLandmarks = LandmarkContainerType::New();
  LandmarkContainerType::Pointer targetLandmarks = LandmarkContainerType::New();

  ImageType::IndexType sourceIndex, targetIndex;

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
    fixedImage->TransformIndexToPhysicalPoint(sourceIndex, sourcePoint);
    movingImage->TransformIndexToPhysicalPoint(targetIndex, targetPoint);

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

  typedef itk::WarpImageFilter< MovingImageType,
                                MovingImageType,
                                DisplacementFieldType  >  FilterType;

  FilterType::Pointer warper = FilterType::New();

  typedef itk::LinearInterpolateImageFunction<
                       MovingImageType, double >  InterpolatorType;

  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  warper->SetInterpolator( interpolator );


  warper->SetOutputSpacing( displacementField->GetSpacing() );
  warper->SetOutputOrigin(  displacementField->GetOrigin() );

  warper->SetDisplacementField( displacementField );

  warper->SetInput( movingReader->GetOutput() );

  movingWriter->SetInput( warper->GetOutput() );

  try
    {
    movingWriter->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  // end landmark warping
  std::cout << "Finished Landmark Warping" << std::endl;
  // proceed to registration

  const unsigned int SpaceDimension = ImageDimension;
  const unsigned int SplineOrder = 3;
  typedef double CoordinateRepType;

  typedef itk::BSplineTransform<
                            CoordinateRepType,
                            SpaceDimension,
                            SplineOrder >     TransformType;

  typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;


  typedef itk::MattesMutualInformationImageToImageMetric<
                                    FixedImageType,
                                    MovingImageType >    MetricType;

  typedef itk::ImageRegistrationMethod<
                                    FixedImageType,
                                    MovingImageType >    RegistrationType;

  MetricType::Pointer         metric        = MetricType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();
  InterpolatorType::Pointer   interpolator2 = InterpolatorType::New();


  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetInterpolator(  interpolator2 );


  TransformType::Pointer  transform = TransformType::New();
  registration->SetTransform( transform );

  // fixedImage carries over from landmark warping
  registration->SetFixedImage(  fixedImage   );
  // carry warper from landmark warping into registration process
  registration->SetMovingImage(   movingWriter->GetInput()   );

  fixedReader->Update();

  FixedImageType::RegionType fixedRegion = fixedImage->GetBufferedRegion();

  registration->SetFixedImageRegion( fixedRegion );

  unsigned int numberOfGridNodesInOneDimension = 7;

  TransformType::PhysicalDimensionsType   fixedPhysicalDimensions;
  TransformType::MeshSizeType             meshSize;
  TransformType::OriginType               fixedOrigin;

  for( unsigned int i=0; i< SpaceDimension; i++ )
    {
    fixedOrigin[i] = fixedImage->GetOrigin()[i];
    fixedPhysicalDimensions[i] = fixedImage->GetSpacing()[i] *
      static_cast<double>(
      fixedImage->GetLargestPossibleRegion().GetSize()[i] - 1 );
    }
  meshSize.Fill( numberOfGridNodesInOneDimension - SplineOrder );

  transform->SetTransformDomainOrigin( fixedOrigin );
  transform->SetTransformDomainPhysicalDimensions(
    fixedPhysicalDimensions );
  transform->SetTransformDomainMeshSize( meshSize );
  transform->SetTransformDomainDirection( fixedImage->GetDirection() );

  typedef TransformType::ParametersType     ParametersType;

  const unsigned int numberOfParameters =
               transform->GetNumberOfParameters();

  ParametersType parameters( numberOfParameters );

  parameters.Fill( 0.0 );

  transform->SetParameters( parameters );

  registration->SetInitialTransformParameters( transform->GetParameters() );

  optimizer->SetMaximumStepLength( 10.0   );
  optimizer->SetMinimumStepLength(  0.01 );

  optimizer->SetRelaxationFactor( 0.7 );
  optimizer->SetNumberOfIterations( atoi(argv[5]) ); // TODO change this number

  // Create the Command observer and register it with the optimizer.
  //
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  metric->SetNumberOfHistogramBins( 50 );

  const unsigned int numberOfSamples =
    static_cast<unsigned int>( fixedRegion.GetNumberOfPixels() * 60.0 / 100.0 );

  metric->SetNumberOfSpatialSamples( numberOfSamples );

  // Add a time probe
  itk::TimeProbesCollectorBase chronometer;
  itk::MemoryProbesCollectorBase memorymeter;

  std::cout << std::endl << "Starting Registration" << std::endl;

  try
    {
    memorymeter.Start( "Registration" );
    chronometer.Start( "Registration" );

    registration->Update();

    chronometer.Stop( "Registration" );
    memorymeter.Stop( "Registration" );

    std::cout << "Optimizer stop condition = "
              << registration->GetOptimizer()->GetStopConditionDescription()
              << std::endl;
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  OptimizerType::ParametersType finalParameters =
                    registration->GetLastTransformParameters();


  // Report the time and memory taken by the registration
  chronometer.Report( std::cout );
  memorymeter.Report( std::cout );

  transform->SetParameters( finalParameters );

  typedef itk::RGBPixel<unsigned char>  ColorPixelType;

  typedef itk::Image< ColorPixelType, ImageDimension > ColorImageType;

  typedef itk::CastImageFilter<
                        ColorImageType,
                        ColorImageType > CastFilterType;

  typedef itk::ImageFileWriter< ColorImageType >  ColorWriterType;

  ColorWriterType::Pointer colorWriter = ColorWriterType::New();
  ColorWriterType::Pointer colorMovingWriter = ColorWriterType::New();

  colorWriter->SetFileName(argv[4]);
  // change this to take a reader as input or something that makes sense TODO
  // colorMovingWriter->SetFileName(argv[3]);

  // colorMovingWriter->Update();

  typedef itk::ResampleImageFilter<
                            ColorImageType,
                            ColorImageType >    ResampleFilterType;

  ResampleFilterType::Pointer resample = ResampleFilterType::New();

  resample->SetTransform( transform );

  resample->SetInput( colorMovingWriter->GetInput() );

  resample->SetSize(    fixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  fixedImage->GetOrigin() );
  resample->SetOutputSpacing( fixedImage->GetSpacing() );
  resample->SetOutputDirection( fixedImage->GetDirection() );

  ColorPixelType defaultPixel;
  defaultPixel[0] = 0;
  defaultPixel[1] = 0;
  defaultPixel[2] = 0;

  resample->SetDefaultPixelValue( defaultPixel );

  

  CastFilterType::Pointer  caster =  CastFilterType::New();


  caster->SetInput( resample->GetOutput() );
  colorWriter->SetInput( caster->GetOutput()   );


  try
    {
    colorWriter->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }

  // typedef itk::SquaredDifferenceImageFilter<
  //                                 FixedImageType,
  //                                 FixedImageType,
  //                                 OutputImageType > DifferenceFilterType;

  // DifferenceFilterType::Pointer difference = DifferenceFilterType::New();

  // WriterType::Pointer writer2 = WriterType::New();
  // writer2->SetInput( difference->GetOutput() );

  // output deformation field to file
  // typedef itk::Vector< float, ImageDimension >      VectorType;
  // typedef itk::Image< VectorType, ImageDimension >  DisplacementFieldType;

  // DisplacementFieldType::Pointer field = DisplacementFieldType::New();
  // field->SetRegions( fixedRegion );
  // field->SetOrigin( fixedImage->GetOrigin() );
  // field->SetSpacing( fixedImage->GetSpacing() );
  // field->SetDirection( fixedImage->GetDirection() );
  // field->Allocate();

  // typedef itk::ImageRegionIterator< DisplacementFieldType > FieldIterator;
  // FieldIterator fi( field, fixedRegion );

  // fi.GoToBegin();

  // TransformType::InputPointType  fixedPoint;
  // TransformType::OutputPointType movingPoint;
  // DisplacementFieldType::IndexType index;

  // VectorType displacement;

  // while( ! fi.IsAtEnd() )
  //   {
  //   index = fi.GetIndex();
  //   field->TransformIndexToPhysicalPoint( index, fixedPoint );
  //   movingPoint = transform->TransformPoint( fixedPoint );
  //   displacement = movingPoint - fixedPoint;
  //   fi.Set( displacement );
  //   ++fi;
  //   }

  // typedef itk::ImageFileWriter< DisplacementFieldType >  FieldWriterType;
  // FieldWriterType::Pointer fieldWriter = FieldWriterType::New();

  // fieldWriter->SetInput( field );

  // // create filename for deformation field
  // // TODO

  // fieldWriter->SetFileName( "deformation.mhd" ); // TODO change to unique filename
  // try
  //   {
  //   fieldWriter->Update();
  //   }
  // catch( itk::ExceptionObject & excp )
  //   {
  //   std::cerr << "Exception thrown " << std::endl;
  //   std::cerr << excp << std::endl;
  //   return EXIT_FAILURE;
  //   }

  std::ofstream parametersFile;
  parametersFile.open( "parameters.txt" );
  parametersFile << finalParameters << std::endl;
  parametersFile.close();

  // end registration
  std::cout << "Finished Registration" << std::endl;

  // read in deformation field file and apply to color image
  // TODO

	return EXIT_SUCCESS;
}