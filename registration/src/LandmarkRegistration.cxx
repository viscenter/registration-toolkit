#include "LandmarkRegistration.h"




BSplineTransformType::Pointer  transform;

ResampleFilterType::Pointer resample;

CastFilterType::Pointer  caster;



RegistrationType::Pointer   registration;

ColorWriterType::Pointer colorMovingWriter;
ColorReaderType::Pointer colorMovingReader;

ColorImageType::ConstPointer colorFixedImage;

bool createVideoFrames;

ColorReaderType::Pointer colorFixedReader = ColorReaderType::New();



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

  // Add a time probe
  itk::TimeProbesCollectorBase chronometer;
  itk::MemoryProbesCollectorBase memorymeter;
  memorymeter.Start( "LandmarkRegistration" );
  chronometer.Start( "LandmarkRegistration" );

  printf("%-17s\n\n", "Landmark Registration");
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

  DisplacementSourceType::Pointer deformer = DisplacementSourceType::New();

  deformer->SetOutputSpacing( grayFixedImage->GetSpacing() );
  deformer->SetOutputOrigin(  grayFixedImage->GetOrigin() );
  deformer->SetOutputRegion(  grayFixedImage->GetLargestPossibleRegion() );
  deformer->SetOutputDirection( grayFixedImage->GetDirection() );

  //  Create source and target landmarks.
  //
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

  GrayFilterType::Pointer grayWarper = GrayFilterType::New();

  GrayInterpolatorType::Pointer grayInterpolator = GrayInterpolatorType::New();

  grayWarper->SetInterpolator( grayInterpolator );


  grayWarper->SetOutputSpacing( displacementField->GetSpacing() );
  grayWarper->SetOutputOrigin(  displacementField->GetOrigin() );

  grayWarper->SetDisplacementField( displacementField );

  grayWarper->SetInput( grayMovingReader->GetOutput() );

  grayMovingWriter->SetInput( grayWarper->GetOutput() );

  ColorFilterType::Pointer colorWarper = ColorFilterType::New();

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
  printf("Finished landmark warping\n");

  // proceed to registration
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
  //
  optimizer->SetGradientMagnitudeTolerance(0.0001);

  // Create the Command observer and register it with the optimizer.
  //
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  metric->SetNumberOfHistogramBins( 50 );

  const unsigned int numberOfSamples =
    // original value in example (very slow and source of much frustration)
    //
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
    //
    static_cast<unsigned int>(grayFixedRegion.GetNumberOfPixels() / 80.0);

  metric->SetNumberOfSpatialSamples( numberOfSamples );

  printf("\nStarting registration\n");

  try
    {
    // memorymeter.Start( "Registration" );
    // chronometer.Start( "Registration" );

    registration->Update();

    // chronometer.Stop( "Registration" );
    // memorymeter.Stop( "Registration" );

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
  // chronometer.Report( std::cout );
  // memorymeter.Report( std::cout );

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
  printf("Finished registration\n\n");

  printf("Writing transformation to file\n");

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

  printf("Finished writing transformation to file\n\n");

  printf("Time and memory usage information:\n");
  chronometer.Stop( "LandmarkRegistration" );
  memorymeter.Stop( "LandmarkRegistration" );
  chronometer.Report( std::cout );
  memorymeter.Report( std::cout );

  return EXIT_SUCCESS;
}
