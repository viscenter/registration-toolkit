#include <iostream>
#include <fstream>
#include <string>

#include "itkRGBPixel.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkTransformFileReader.h"
#include "itkTransform.h"
#include "itkTransformFactoryBase.h"
#include "itkTransformFactory.h"
#include "itkBSplineTransform.h"
#include "itkLandmarkDisplacementFieldSource.h"
#include "itkVector.h"
#include "itkIndex.h"
#include "itkWarpVectorImageFilter.h"
#include "itkImageRegion.h"
#include "itkVectorLinearInterpolateImageFunction.h"

int main(int argc, char* argv[])
{
    if(argc < 4)
    {
        std::cerr << "Missing Parameters " << std::endl;
        std::cerr << "Usage: " << argv[0];
        std::cerr << " transformationFile movingImage ";
        std::cerr << "outputImage" << std::endl;

        return EXIT_FAILURE;
    }

    std::cout << std::endl << "Beginning transformation" << std::endl;

    const unsigned int ImageDimension = 2;
    typedef itk::RGBPixel<unsigned char> PixelType;
    typedef itk::Image<PixelType, ImageDimension> ImageType;

    typedef itk::ImageFileReader<ImageType> ReaderType;
    typedef itk::ImageFileWriter<ImageType> WriterType;

    typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleFilterType;
    typedef itk::CastImageFilter<ImageType, ImageType> CastFilterType;

    itk::TransformFileReaderTemplate<double>::Pointer transformReader = itk::TransformFileReaderTemplate<double>::New();
    transformReader->SetFileName(argv[1]);

    itk::TransformFactoryBase::RegisterDefaultTransforms();

    typedef itk::BSplineTransform<double, ImageDimension> BSplineTransformType;

    typedef itk::VectorLinearInterpolateImageFunction<ImageType, double>  InterpolatorType;

    try
        {
        transformReader->Update();
        }
    catch( itk::ExceptionObject & excp )
        {
        std::cerr << "Transform Reader exception thrown " << std::endl;
        std::cerr << excp << std::endl;
        return EXIT_FAILURE;
        }

    ReaderType::Pointer reader = ReaderType::New();
    WriterType::Pointer writer = WriterType::New();

    reader->SetFileName(argv[2]);
    writer->SetFileName(argv[3]);

    try
        {
        reader->Update();
        }
    catch( itk::ExceptionObject & excp )
        {
        std::cerr << "Image Reader exception thrown " << std::endl;
        std::cerr << excp << std::endl;
        return EXIT_FAILURE;
        }

    ImageType::ConstPointer readImage = reader->GetOutput();

    typedef itk::TransformFileReader::TransformListType * TransformListType;
    TransformListType transforms = transformReader->GetTransformList();

    itk::TransformFileReader::TransformListType::const_iterator it = transforms->begin();
    
    BSplineTransformType::Pointer bspline_read;
    if(!strcmp((*it)->GetNameOfClass(),"BSplineTransform"))
        {
        bspline_read = static_cast<BSplineTransformType*>((*it).GetPointer());
        // bspline_read->Print(std::cout);
        }

    std::ifstream transformFile;
    transformFile.open(argv[1]);
    std::string line;
    std::getline(transformFile, line);
    while(line != "#Fixed image parameters")
    {
        std::getline(transformFile, line);
    }
    int i, j, k, l;

    transformFile >> line >> i >> j;
    ImageType::SizeType size;
    size[0] = i;
    size[1] = j;
    // std::cout << size << std::endl;

    transformFile >> line >> i >> j;
    int origin[2];
    origin[0] = i;
    origin[1] = j;
    // std::cout << origin << std::endl;

    transformFile >> line >> i >> j;
    ImageType::SpacingType spacing;
    spacing[0] = i;
    spacing[1] = j;
    // std::cout << spacing << std::endl;

    transformFile >> line >> i >> j >> k >> l;
    ImageType::DirectionType direction;
    direction[0][0] = i;
    direction[0][1] = j;
    direction[1][0] = k;
    direction[1][1] = l;
    // std::cout << direction << std::endl;

    PixelType defaultPixel;
    defaultPixel[0] = 0;
    defaultPixel[1] = 0;
    defaultPixel[2] = 0;

    typedef   float          VectorComponentType;
    typedef   itk::Vector< VectorComponentType, ImageDimension >    VectorType;
    typedef   itk::Image< VectorType,  ImageDimension >   DisplacementFieldType;
    typedef itk::LandmarkDisplacementFieldSource<
                                DisplacementFieldType
                                             >  DisplacementSourceType;

    DisplacementSourceType::Pointer deformer = DisplacementSourceType::New();

    deformer->SetOutputSpacing( spacing );
    deformer->SetOutputOrigin(  origin );

    itk::Index<ImageDimension> index;
    index.Fill(0);
    itk::ImageRegion<ImageDimension> region(index, size);

    deformer->SetOutputRegion(  region );
    deformer->SetOutputDirection( direction );

    typedef DisplacementSourceType::LandmarkContainerPointer   LandmarkContainerPointer;
    typedef DisplacementSourceType::LandmarkContainer          LandmarkContainerType;
    typedef DisplacementSourceType::LandmarkPointType          LandmarkPointType;

    LandmarkContainerType::Pointer sourceLandmarks = LandmarkContainerType::New();
    LandmarkContainerType::Pointer targetLandmarks = LandmarkContainerType::New();

    ImageType::IndexType sourceIndex, targetIndex;

    LandmarkPointType sourcePoint, targetPoint;

    while(line != "#Landmark warping physical points")
    {
        std::getline(transformFile, line);
    }

    unsigned int pointId = 0;

    while(!transformFile.fail())
    {
        transformFile >> line >> sourcePoint >> targetPoint;

        sourceLandmarks->InsertElement( pointId, sourcePoint );
        targetLandmarks->InsertElement( pointId, targetPoint );
        pointId++;
    }

    transformFile.close();

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

    typedef itk::WarpVectorImageFilter< ImageType,
                                ImageType,
                                DisplacementFieldType  >  FilterType;

    FilterType::Pointer warper = FilterType::New();

    InterpolatorType::Pointer interpolator = InterpolatorType::New();

    warper->SetInterpolator( interpolator );

    warper->SetCoordinateTolerance(10);

    warper->SetOutputSpacing( displacementField->GetSpacing() );
    warper->SetOutputOrigin(  displacementField->GetOrigin() );

    warper->SetDisplacementField( displacementField );

    warper->SetInput( reader->GetOutput() );

    writer->SetInput( warper->GetOutput() );

    try
    {
        writer->Update();
    }
    catch(itk::ExceptionObject & err)
    {
        std::cerr << "Image writer exception caught!" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Applied landmark warping" << std::endl;

    ResampleFilterType::Pointer resample = ResampleFilterType::New();

    resample->SetTransform(bspline_read);

    resample->SetInput(writer->GetInput());

    resample->SetSize(size);
    resample->SetOutputOrigin(origin);
    resample->SetOutputSpacing(spacing);
    resample->SetOutputDirection(direction);
    resample->SetDefaultPixelValue(defaultPixel);

    CastFilterType::Pointer caster = CastFilterType::New();

    caster->SetInput(resample->GetOutput());
    writer->SetInput(caster->GetOutput());

    try
        {
        writer->Update();
        }
    catch( itk::ExceptionObject & err )
        {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
        }

    std::cout << "Applied registration transform" << std::endl;

    std::cout << "Finished applying transformations" << std::endl << std::endl;

    return EXIT_SUCCESS;
}