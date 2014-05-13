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
#include "itkKernelTransform.h"
#include "itkRegularStepGradientDescentOptimizer.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkSquaredDifferenceImageFilter.h"

#include "itkRGBPixel.h"

#include "itkTransformFileWriter.h"

#include <fstream>
#include <string>
#include <iostream>

//  The following section of code implements a Command observer
//  used to monitor the evolution of the registration process.

const     unsigned int   ImageDimension = 2;
const unsigned int SpaceDimension = ImageDimension;
const unsigned int SplineOrder = 3;
typedef double CoordinateRepType;

typedef itk::BSplineTransform<
                          CoordinateRepType,
                          SpaceDimension,
                          SplineOrder >     BSplineTransformType;

typedef BSplineTransformType::ParametersType     BSplineParametersType;

BSplineTransformType::Pointer  transform;

typedef itk::RGBPixel<unsigned char>  ColorPixelType;

typedef itk::Image< ColorPixelType, ImageDimension > ColorImageType;

typedef itk::ResampleImageFilter<
                          ColorImageType,
                          ColorImageType >    ResampleFilterType;

ResampleFilterType::Pointer resample;

typedef itk::CastImageFilter<
                      ColorImageType,
                      ColorImageType > CastFilterType;

CastFilterType::Pointer  caster;

typedef itk::ImageFileReader< ColorImageType >  ColorReaderType;
typedef itk::ImageFileWriter< ColorImageType >  ColorWriterType;

ColorReaderType::Pointer colorFixedReader = ColorReaderType::New();

typedef   unsigned char GrayPixelType;

typedef itk::Image<GrayPixelType, ImageDimension> GrayImageType;

typedef itk::ImageRegistrationMethod<
                                    GrayImageType,
                                    GrayImageType >    RegistrationType;

RegistrationType::Pointer   registration;

ColorWriterType::Pointer colorMovingWriter;
ColorReaderType::Pointer colorMovingReader;

ColorImageType::ConstPointer colorFixedImage;

bool createVideoFrames;

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

    if(createVideoFrames)
      {
      BSplineParametersType registrationParameters =
                      optimizer->GetCurrentPosition();

      transform->SetParameters( registrationParameters );

      ResampleFilterType::Pointer resample = ResampleFilterType::New();

      resample->SetTransform( transform );

      resample->SetInput( colorMovingWriter->GetInput() );

      resample->SetSize(    colorFixedImage->GetLargestPossibleRegion().GetSize() );
      resample->SetOutputOrigin(  colorFixedImage->GetOrigin() );
      resample->SetOutputSpacing( colorFixedImage->GetSpacing() );
      resample->SetOutputDirection( colorFixedImage->GetDirection() );

      ColorPixelType defaultPixel;
      defaultPixel[0] = 0;
      defaultPixel[1] = 0;
      defaultPixel[2] = 0;

      resample->SetDefaultPixelValue( defaultPixel );

      ColorWriterType::Pointer videoFrameWriter = ColorWriterType::New();

      CastFilterType::Pointer caster =  CastFilterType::New();

      char outfilename[21];
      sprintf(outfilename,"registered-%03d.jpg",optimizer->GetCurrentIteration());

      videoFrameWriter->SetFileName(outfilename);
      
      caster->SetInput( resample->GetOutput() );
      videoFrameWriter->SetInput( caster->GetOutput()   );

      try
        {
        videoFrameWriter->Update();
        }
      catch( itk::ExceptionObject & err )
        {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        // return EXIT_FAILURE;
        }
      }
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
    std::cerr << "numberOfIterations ";
    std::cerr << "[createVideoFrames]" << std::endl;
    return EXIT_FAILURE;
    }

  std::printf("%-17s\n\n", "Landmark Registration");
  std::printf("%-17s %s\n", "Landmarks file: ", argv[1]);
  std::printf("%-17s %s\n", "Fixed image: ", argv[2]);
  std::printf("%-17s %s\n", "Moving image: ", argv[3]);
  std::printf("%-17s %s\n", "Output image: ", argv[4]);
  std::printf("%-17s %s\n", "Iterations: ", argv[5]);
  if(argc > 6)
  {
    std::printf("%-17s %s\n", "Video frames: ", "Yes");
  }
  else
  {
    std::printf("%-17s %s\n", "Video frames: ", "No");
  }

  std::cout << std::endl << "Starting Landmark Warping" << std::endl;

  typedef   float          VectorComponentType;

  typedef itk::KernelTransform< double, ImageDimension > KernelTransformType;
  typedef KernelTransformType::ParametersType KernelParametersType;

  typedef   itk::Vector< VectorComponentType, ImageDimension >    VectorType;

  typedef   itk::Image< VectorType,  ImageDimension >   DisplacementFieldType;

  typedef   itk::ImageFileReader< GrayImageType >  GrayReaderType;
  typedef   itk::ImageFileWriter< GrayImageType >  GrayWriterType;

  GrayReaderType::Pointer grayFixedReader = GrayReaderType::New();
  ColorReaderType::Pointer colorFixedReader = ColorReaderType::New();
  grayFixedReader->SetFileName( argv[2] );
  colorFixedReader->SetFileName( argv[2] );

  try
    {
    grayFixedReader->Update();
    colorFixedReader->Update();
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
  colorFixedImage = colorFixedReader->GetOutput();
  GrayImageType::ConstPointer grayMovingImage = grayMovingReader->GetOutput();

  colorMovingReader = ColorReaderType::New();
  colorMovingWriter = ColorWriterType::New();

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

  KernelTransformType::Pointer kernelTransform = deformer->GetModifiableKernelTransform();

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

  // end landmark warping
  std::cout << "Finished Landmark Warping" << std::endl;
  // proceed to registration

  typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;


  typedef itk::MattesMutualInformationImageToImageMetric<
                                    GrayImageType,
                                    GrayImageType >    MetricType;

  MetricType::Pointer         metric        = MetricType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  registration  = RegistrationType::New();


  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetInterpolator(  grayInterpolator );


  transform = BSplineTransformType::New();
  registration->SetTransform( transform );

  // the gray image is used for registration calculations
  // the resulting transform is then applied to the color image

  // fixedImage carries over from landmark warping
  registration->SetFixedImage(  grayFixedImage   );
  // carry warper from landmark warping into registration process
  grayMovingReader->SetFileName(argv[4]);
  grayMovingReader->Update();
  registration->SetMovingImage(   grayMovingReader->GetOutput()  );

  grayFixedReader->Update();

  GrayImageType::RegionType grayFixedRegion = grayFixedImage->GetBufferedRegion();

  registration->SetFixedImageRegion( grayFixedRegion );

  unsigned int numberOfGridNodesInOneDimension = 7;

  BSplineTransformType::PhysicalDimensionsType   grayFixedPhysicalDimensions;
  BSplineTransformType::MeshSizeType             grayMeshSize;
  BSplineTransformType::OriginType               grayFixedOrigin;

  for( unsigned int i=0; i< SpaceDimension; i++ )
    {
    grayFixedOrigin[i] = grayFixedImage->GetOrigin()[i];
    grayFixedPhysicalDimensions[i] = grayFixedImage->GetSpacing()[i] *
      static_cast<double>(
      grayFixedImage->GetLargestPossibleRegion().GetSize()[i] - 1 );
    }

  /* Below changed to increase flexibility of transform which
    completely changed the success of the registration for the better */

  // grayMeshSize.Fill( numberOfGridNodesInOneDimension - SplineOrder );
  grayMeshSize.Fill(12);

  transform->SetTransformDomainOrigin( grayFixedOrigin );
  transform->SetTransformDomainPhysicalDimensions(
    grayFixedPhysicalDimensions );
  transform->SetTransformDomainMeshSize( grayMeshSize );
  transform->SetTransformDomainDirection( grayFixedImage->GetDirection() );

  const unsigned int numberOfParameters =
               transform->GetNumberOfParameters();

  BSplineParametersType parameters( numberOfParameters );

  parameters.Fill( 0.0 );

  transform->SetParameters( parameters );

  registration->SetInitialTransformParameters( transform->GetParameters() );

  optimizer->SetMaximumStepLength( grayFixedImage->GetLargestPossibleRegion().GetSize()[0] / 500.0 );
  optimizer->SetMinimumStepLength(  0.01 );

  optimizer->SetRelaxationFactor( 0.85 );
  optimizer->SetNumberOfIterations( atoi(argv[5]) );

  // Stop before 100 iterations if things are going well
  optimizer->SetGradientMagnitudeTolerance(0.0001);

  // Create the Command observer and register it with the optimizer.
  //
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  // create video frames if asked for
  if(argc > 6)
  {
    createVideoFrames = true;
  }
  else
  {
    createVideoFrames = false;
  }

  metric->SetNumberOfHistogramBins( 50 );

  const unsigned int numberOfSamples =
    static_cast<unsigned int>( grayFixedRegion.GetNumberOfPixels() * 60.0 / 100.0 );

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

  BSplineParametersType registrationParameters =
                    registration->GetLastTransformParameters();


  // Report the time and memory taken by the registration
  chronometer.Report( std::cout );
  memorymeter.Report( std::cout );

  transform->SetParameters( registrationParameters );

  resample = ResampleFilterType::New();

  resample->SetTransform( transform );

  resample->SetInput( colorMovingWriter->GetInput() );

  resample->SetSize(    colorFixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(  colorFixedImage->GetOrigin() );
  resample->SetOutputSpacing( colorFixedImage->GetSpacing() );
  resample->SetOutputDirection( colorFixedImage->GetDirection() );

  ColorPixelType defaultPixel;
  defaultPixel[0] = 0;
  defaultPixel[1] = 0;
  defaultPixel[2] = 0;

  resample->SetDefaultPixelValue( defaultPixel );

  caster =  CastFilterType::New();

  caster->SetInput( resample->GetOutput() );
  colorMovingWriter->SetInput( caster->GetOutput()   );

  try
    {
    colorMovingWriter->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cerr << "ExceptionObject caught !" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
    }
  
  // end registration
  std::cout << "Finished Registration" << std::endl << std::endl;

  std::cout << "Writing Transformation to File" << std::endl;

  std::string transformFileName = "transform.tfm";

  itk::TransformFileWriterTemplate<double>::Pointer transformWriter = itk::TransformFileWriterTemplate<double>::New();
  transformWriter->SetFileName(transformFileName);
  transformWriter->SetInput(transform);
  // transformWriter->AddTransform(transform);
  transformWriter->Update();

  std::ofstream transformFile;
  transformFile.open(transformFileName.c_str(), std::ios::app);
  transformFile << std::endl;
  transformFile << "#Fixed image parameters" 
    << std::endl;
  transformFile << "#Size " 
    << colorFixedImage->GetLargestPossibleRegion().GetSize()[0]
    << " " << colorFixedImage->GetLargestPossibleRegion().GetSize()[1] 
    << std::endl;
  transformFile << "#Origin " 
    << colorFixedImage->GetOrigin()[0]
    << " " << colorFixedImage->GetOrigin()[1] 
    << std::endl;
  transformFile << "#Spacing " 
    << colorFixedImage->GetSpacing()[0]
    << " " << colorFixedImage->GetSpacing()[1] 
    << std::endl;
  transformFile << "#Direction " 
    << colorFixedImage->GetDirection()[0][0]
    << " " << colorFixedImage->GetDirection()[0][1]
    << " " << colorFixedImage->GetDirection()[1][0]
    << " " << colorFixedImage->GetDirection()[1][1] 
    << std::endl << std::endl;

  transformFile << "#Landmark warping physical points" << std::endl;

  pointsFile.open( argv[1] );

  pointsFile >> sourceX >> sourceY >> targetX >> targetY;

  sourceIndex[0] = sourceX;
  sourceIndex[1] = sourceY;
  targetIndex[0] = targetX;
  targetIndex[1] = targetY;

  grayFixedImage->TransformIndexToPhysicalPoint(sourceIndex, sourcePoint);
  grayMovingImage->TransformIndexToPhysicalPoint(targetIndex, targetPoint);

  transformFile << "# " << sourcePoint[0] << " " << sourcePoint[1] << " " << targetPoint[0] << " " << targetPoint[1] << std::endl;

  while( !pointsFile.fail() )
    {
    pointsFile >> sourceX >> sourceY >> targetX >> targetY;

    sourceIndex[0] = sourceX;
    sourceIndex[1] = sourceY;
    targetIndex[0] = targetX;
    targetIndex[1] = targetY;

    grayFixedImage->TransformIndexToPhysicalPoint(sourceIndex, sourcePoint);
    grayMovingImage->TransformIndexToPhysicalPoint(targetIndex, targetPoint);

    transformFile << "# " << sourcePoint[0] << " " << sourcePoint[1] << " " << targetPoint[0] << " " << targetPoint[1] << std::endl;
    }

  pointsFile.close();

  transformFile.close();

  std::cout << "Finished Writing Transformation to File" << std::endl << std::endl;


  return EXIT_SUCCESS;
}