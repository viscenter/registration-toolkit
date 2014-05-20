#include "LandmarkRegistration.h"

BSplineTransformType::Pointer transform;
ResampleFilterType::Pointer   resample;
CastFilterType::Pointer       caster;
RegistrationType::Pointer     registration;

ColorWriterType::Pointer colorMovingWriter;
ColorReaderType::Pointer colorMovingReader;

ColorReaderType::Pointer colorFixedReader = ColorReaderType::New();
ColorImageType::ConstPointer colorFixedImage;

bool createVideoFrames;

class CommandIterationUpdate : public itk::Command
{
public:
  typedef CommandIterationUpdate    Self;
  typedef itk::Command              Superclass;
  typedef itk::SmartPointer<Self>   Pointer;
  itkNewMacro( Self );

protected:
  CommandIterationUpdate() {};

public:
  typedef itk::RegularStepGradientDescentOptimizer OptimizerType;
  typedef const OptimizerType *                    OptimizerPointer;

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

      ColorPixelType defaultPixel;
      defaultPixel[0] = 0;
      defaultPixel[1] = 0;
      defaultPixel[2] = 0;

      resample->SetTransform(         transform                                             );
      resample->SetInput(             colorMovingWriter->GetInput()                         );
      resample->SetSize(              colorFixedImage->GetLargestPossibleRegion().GetSize() );
      resample->SetOutputOrigin(      colorFixedImage->GetOrigin()                          );
      resample->SetOutputSpacing(     colorFixedImage->GetSpacing()                         );
      resample->SetOutputDirection(   colorFixedImage->GetDirection()                       );
      resample->SetDefaultPixelValue( defaultPixel                                          );

      ColorWriterType::Pointer videoFrameWriter = ColorWriterType::New();

      CastFilterType::Pointer caster =  CastFilterType::New();

      char outfilename[128];
      // TODO change this to use the output image name with numbering (same name as transforms etc)
      sprintf(outfilename,"registered-%03d.jpg",optimizer->GetCurrentIteration());

      caster->SetInput(              resample->GetOutput() );
      videoFrameWriter->SetFileName( outfilename );
      videoFrameWriter->SetInput(    caster->GetOutput() );

      try
        {
        videoFrameWriter->Update();
        }
      catch( itk::ExceptionObject & err )
        {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
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

  // Add time and memory probes for entire program
  itk::TimeProbesCollectorBase chronometer;
  itk::MemoryProbesCollectorBase memorymeter;
  memorymeter.Start( "LandmarkRegistration" );
  chronometer.Start( "LandmarkRegistration" );

  printf("%-17s\n\n",  "Landmark Registration");
  printf("%-17s %s\n", "Landmarks file: ", argv[1]);
  printf("%-17s %s\n", "Fixed image: ", argv[2]);
  printf("%-17s %s\n", "Moving image: ", argv[3]);
  printf("%-17s %s\n", "Output image: ", argv[4]);
  printf("%-17s %s\n", "Iterations: ", argv[5]);
  if(argc > 6)
  {
    printf("%-17s %s\n", "Video frames: ", "Yes");
    createVideoFrames = true;
  }
  else
  {
    printf("%-17s %s\n", "Video frames: ", "No");
    createVideoFrames = false;
  }

  printf("\nStarting landmark warping\n");

  GrayReaderType::Pointer  grayFixedReader  = GrayReaderType::New();
  ColorReaderType::Pointer colorFixedReader = ColorReaderType::New();
  GrayReaderType::Pointer  grayMovingReader = GrayReaderType::New();
  GrayWriterType::Pointer  grayMovingWriter = GrayWriterType::New();
  colorMovingReader                         = ColorReaderType::New();
  colorMovingWriter                         = ColorWriterType::New();

  grayFixedReader->SetFileName(   argv[2] );
  colorFixedReader->SetFileName(  argv[2] );
  grayMovingReader->SetFileName(  argv[3] );
  colorMovingReader->SetFileName( argv[3] );
  colorMovingWriter->SetFileName( argv[4] );

  try
    {
    grayFixedReader->Update();
    colorFixedReader->Update();
    grayMovingReader->Update();
    colorMovingReader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  GrayImageType::ConstPointer  grayFixedImage   = grayFixedReader->GetOutput();
  GrayImageType::ConstPointer  grayMovingImage  = grayMovingReader->GetOutput();
  colorFixedImage                               = colorFixedReader->GetOutput();
  ColorImageType::ConstPointer colorMovingImage = colorMovingReader->GetOutput();

  DisplacementSourceType::Pointer deformer = DisplacementSourceType::New();

  deformer->SetOutputSpacing(   grayFixedImage->GetSpacing()               );
  deformer->SetOutputOrigin(    grayFixedImage->GetOrigin()                );
  deformer->SetOutputRegion(    grayFixedImage->GetLargestPossibleRegion() );
  deformer->SetOutputDirection( grayFixedImage->GetDirection()             );

  //  Create source and target landmarks.
  LandmarkContainerType::Pointer sourceLandmarks = LandmarkContainerType::New();
  LandmarkContainerType::Pointer targetLandmarks = LandmarkContainerType::New();

  GrayImageType::IndexType sourceIndex, targetIndex;
  LandmarkPointType        sourcePoint, targetPoint;

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
    grayFixedImage->TransformIndexToPhysicalPoint(  sourceIndex, sourcePoint );
    grayMovingImage->TransformIndexToPhysicalPoint( targetIndex, targetPoint );

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

  KernelTransformType::Pointer        kernelTransform   = deformer->GetModifiableKernelTransform();
  DisplacementFieldType::ConstPointer displacementField = deformer->GetOutput();
  GrayFilterType::Pointer             grayWarper        = GrayFilterType::New();
  GrayInterpolatorType::Pointer       grayInterpolator  = GrayInterpolatorType::New();

  grayWarper->SetInterpolator(      grayInterpolator                );
  grayWarper->SetOutputSpacing(     displacementField->GetSpacing() );
  grayWarper->SetOutputOrigin(      displacementField->GetOrigin()  );
  grayWarper->SetDisplacementField( displacementField               );
  grayWarper->SetInput(             grayMovingReader->GetOutput()   );

  grayMovingWriter->SetInput( grayWarper->GetOutput() );

  ColorFilterType::Pointer       colorWarper       = ColorFilterType::New();
  ColorInterpolatorType::Pointer colorInterpolator = ColorInterpolatorType::New();

  // necessary to avoid "input images do not occupy the same physical space" error
  // not necessary in gray image for some reason
  // with color images this gives the desired output colored image
  //
  colorWarper->SetCoordinateTolerance( 10.0                            );
  //
  colorWarper->SetInterpolator(        colorInterpolator               );
  colorWarper->SetOutputSpacing(       displacementField->GetSpacing() );
  colorWarper->SetOutputOrigin(        displacementField->GetOrigin()  );
  colorWarper->SetDisplacementField(   displacementField               );
  colorWarper->SetInput(               colorMovingReader->GetOutput()  );

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

  printf("Finished landmark warping\n");

  // proceed to registration
  MetricType::Pointer         metric        = MetricType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  registration                              = RegistrationType::New();
  transform                                 = BSplineTransformType::New();

  grayMovingReader->SetFileName(argv[4]);
  grayMovingReader->Update();
  grayFixedReader->Update();

  registration->SetMetric(       metric           );
  registration->SetOptimizer(    optimizer        );
  registration->SetInterpolator( grayInterpolator );
  registration->SetTransform(    transform        );
  registration->SetFixedImage(   grayFixedImage   );
  registration->SetMovingImage(  grayMovingReader->GetOutput() );


  GrayImageType::RegionType grayFixedRegion = grayFixedImage->GetBufferedRegion();
  registration->SetFixedImageRegion( grayFixedRegion );

  unsigned int numberOfGridNodesInOneDimension = 7;

  //////////////////////////////////////////////////////////////////////////////////////////

  // Adjust these values to modify the registration process

  // Initially numberOfGridNodesInOneDimension - SplineOrder = 7 - 3 = 4
  // Increased to improve transform flexibility
  int transformMeshFillSize = 12;

  // The maximum step length when the optimizer starts moving around
  double maximumStepLength = grayFixedImage->GetLargestPossibleRegion().GetSize()[0] / 500.0;
  // Registration will stop if the step length drops below this value
  double minimumStepLength = grayFixedImage->GetLargestPossibleRegion().GetSize()[0] / 10000.0;

  // Optimizer step length is reduced by this factor each iteration
  double relaxationFactor = 0.85;
  // The registration process will stop by this many iterations if it has not already
  int numberOfIterations = atoi(argv[5]);
  // The registration process will stop if the metric starts changing less than this
  double gradientMagnitudeTolerance = 0.0001;

  int numberOfHistogramBins = 50;

  unsigned int numberOfSamples =
    // original value in example (very slow and source of much frustration)
    // static_cast<unsigned int>( grayFixedRegion.GetNumberOfPixels() * 60.0 / 100.0 );

    //  http://www.itk.org/Insight/Doxygen/html/Registration_2ImageRegistration16_8cxx-example.html
    //  The metric requires two parameters to be selected: the number
    //  of bins used to compute the entropy and the number of spatial samples
    //  used to compute the density estimates. In typical application, 50
    //  histogram bins are sufficient and the metric is relatively insensitive
    //  to changes in the number of bins. The number of spatial samples
    //  to be used depends on the content of the image. If the images are
    //  smooth and do not contain much detail, then using approximately
    //  1 percent of the pixels will do. On the other hand, if the images
    //  are detailed, it may be necessary to use a much higher proportion,
    //  such as 20 percent.
    static_cast<unsigned int>(grayFixedRegion.GetNumberOfPixels() / 80.0);

  //////////////////////////////////////////////////////////////////////////////////////////
  
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

  grayMeshSize.Fill( transformMeshFillSize );

  transform->SetTransformDomainOrigin(             grayFixedOrigin                );
  transform->SetTransformDomainPhysicalDimensions( grayFixedPhysicalDimensions    );
  transform->SetTransformDomainMeshSize(           grayMeshSize                   );
  transform->SetTransformDomainDirection(          grayFixedImage->GetDirection() );

  const unsigned int numberOfParameters = transform->GetNumberOfParameters();
  BSplineParametersType parameters( numberOfParameters );
  parameters.Fill( 0.0 );
  transform->SetParameters( parameters );
  registration->SetInitialTransformParameters( transform->GetParameters() );

  optimizer->SetMaximumStepLength(          maximumStepLength          );
  optimizer->SetMinimumStepLength(          minimumStepLength          );
  optimizer->SetRelaxationFactor(           relaxationFactor           );  
  optimizer->SetNumberOfIterations(         numberOfIterations         );
  optimizer->SetGradientMagnitudeTolerance( gradientMagnitudeTolerance );

  // Create the Command observer and register it with the optimizer.
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  metric->SetNumberOfHistogramBins( numberOfHistogramBins );
  metric->SetNumberOfSpatialSamples( numberOfSamples );

  printf("\nStarting registration\n");

  try
    {
    registration->Update();

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
  transform->SetParameters( registrationParameters );

  resample = ResampleFilterType::New();

  ColorPixelType defaultPixel;
  defaultPixel[0] = 0;
  defaultPixel[1] = 0;
  defaultPixel[2] = 0;

  resample->SetTransform(         transform                                             );
  resample->SetInput(             colorMovingWriter->GetInput()                         );
  resample->SetSize(              colorFixedImage->GetLargestPossibleRegion().GetSize() );
  resample->SetOutputOrigin(      colorFixedImage->GetOrigin()                          );
  resample->SetOutputSpacing(     colorFixedImage->GetSpacing()                         );
  resample->SetOutputDirection(   colorFixedImage->GetDirection()                       );
  resample->SetDefaultPixelValue( defaultPixel                                          );

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
  
  printf("Finished registration\n\n");
  printf("Writing transformation to file\n");

  std::string transformFileName = "transform.tfm";

  itk::TransformFileWriterTemplate<double>::Pointer transformWriter = itk::TransformFileWriterTemplate<double>::New();
  transformWriter->SetFileName(transformFileName);
  transformWriter->SetInput(transform);
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

  printf("Finished writing transformation to file\n\n");

  printf("Time and memory usage information:\n");
  chronometer.Stop( "LandmarkRegistration" );
  memorymeter.Stop( "LandmarkRegistration" );
  chronometer.Report( std::cout );
  memorymeter.Report( std::cout );

  return EXIT_SUCCESS;
}
