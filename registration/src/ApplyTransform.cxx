#include <iostream>
#include <fstream>
#include <string>

#include "itkRGBPixel.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCompositeTransform.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkTransformFileReader.h"
#include "itkTransform.h"
#include "itkTransformFactoryBase.h"
#include "itkTransformFactory.h"
#include "itkBSplineTransform.h"

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

    std::cout << std::endl << "Beginning Transformation" << std::endl;

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

    typedef itk::LinearInterpolateImageFunction<ImageType, double>  InterpolatorType;

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

    typedef itk::TransformFileReader::TransformListType * TransformListType;
    TransformListType transforms = transformReader->GetTransformList();
    std::cout << "Number of transforms = " << transforms->size() << std::endl;

    itk::TransformFileReader::TransformListType::const_iterator it = transforms->begin();
    
    BSplineTransformType::Pointer bspline_read;
    if(!strcmp((*it)->GetNameOfClass(),"BSplineTransform"))
        {
        bspline_read = static_cast<BSplineTransformType*>((*it).GetPointer());
        bspline_read->Print(std::cout);
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
    std::cout << size << std::endl;

    transformFile >> line >> i >> j;
    int origin[2];
    origin[0] = i;
    origin[1] = j;
    std::cout << origin << std::endl;

    transformFile >> line >> i >> j;
    ImageType::SpacingType spacing;
    spacing[0] = i;
    spacing[1] = j;
    std::cout << spacing << std::endl;

    transformFile >> line >> i >> j >> k >> l;
    ImageType::DirectionType direction;
    direction[0][0] = i;
    direction[0][1] = j;
    direction[1][0] = k;
    direction[1][1] = l;
    std::cout << direction << std::endl;

    PixelType defaultPixel;
    defaultPixel[0] = 0;
    defaultPixel[1] = 0;
    defaultPixel[2] = 0;

    while(line != "#Landmark warping physical points")
    {
        std::getline(transformFile, line);
    }
    while(!transformFile.fail())
    {
        transformFile >> line >> i >> j >> k >> l;
    }

    transformFile.close();

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

    std::cout << "Finished Applying Transformation" << std::endl << std::endl;

    return EXIT_SUCCESS;
}