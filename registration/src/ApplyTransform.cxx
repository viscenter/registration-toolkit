#include <iostream>
#include <fstream>

#include "itkRGBPixel.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCompositeTransform.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkTransformFileReader.h"
#include "itkTransform.h"

int main(int argc, char* argv[])
{
    if(argc < 4)
    {
        std::cerr << "Missing Parameters " << std::endl;
        std::cerr << "Usage: " << argv[0];
        std::cerr << " transformationFile fixedImage ";
        std::cerr << "movingImage" << std::endl;

        return EXIT_FAILURE;
    }

    std::cout << std::endl << "Beginning Transformation" << std::endl;

    const unsigned int ImageDimension = 2;
    typedef itk::RGBPixel<unsigned char> PixelType;
    typedef itk::Image<PixelType, ImageDimension> ImageType;

    typedef itk::ImageFileReader<ImageType> ReaderType;
    typedef itk::ImageFileWriter<ImageType> WriterType;

    typedef itk::CompositeTransform<double, ImageDimension> CompositeTransformType;

    typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleFilterType;
    typedef itk::CastImageFilter<ImageType, ImageType> CastFilterType;

    itk::TransformFileReaderTemplate<double>::Pointer transformReader = 
        itk::TransformFileReaderTemplate<double>::New();
    transformReader->SetFileName(argv[1]);

    try
        {
        transformReader->Update();
        }
    catch( itk::ExceptionObject & excp )
        {
        std::cerr << "Exception thrown " << std::endl;
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
        std::cerr << "Exception thrown " << std::endl;
        std::cerr << excp << std::endl;
        return EXIT_FAILURE;
        }

    ImageType::ConstPointer fixedImage = reader->GetOutput();

    CompositeTransformType::Pointer transform = CompositeTransformType::New();
    itk::TransformFileReaderTemplate<double>::TransformListType transformList = 
        *(transformReader->GetTransformList());

    // itk::TransformFileReaderTemplate<double>::TransformListType::iterator iterator;
    // for(iterator = transformList.begin(); iterator != transformList.end(); ++iterator)
    // {
    //     transform->AddTransform(*iterator);
    // }

    std::cout << *(transformList.begin()) << std::endl;

    // std::ifstream parametersFile;
    // parametersFile.open(argv[1]);
    // parametersFile >> parameters;
    // parametersFile.close();

    // transform->SetParameters(parameters);

    ResampleFilterType::Pointer resample = ResampleFilterType::New();
    resample->SetTransform(transform);

    resample->SetInput(writer->GetInput());

    PixelType defaultPixel;
    defaultPixel[0] = 0;
    defaultPixel[1] = 0;
    defaultPixel[2] = 0;

    resample->SetDefaultPixelValue( defaultPixel );

    CastFilterType::Pointer caster = CastFilterType::New();
    caster->SetInput(resample->GetOutput());
    writer->SetInput(caster->GetOutput());

    try
    {
        writer->Update();
    }
    catch(itk::ExceptionObject & err)
    {
        std::cerr << "ExceptionObject caught!" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Finished Applying Transformation" << std::endl << std::endl;

    return EXIT_SUCCESS;
}