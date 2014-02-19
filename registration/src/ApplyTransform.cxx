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
#include "itkTransformFactoryBase.h"
#include "itkTransformFactory.h"
#include "itkThinPlateSplineKernelTransform.h"
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

    typedef itk::CompositeTransform<double, ImageDimension> CompositeTransformType;

    typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleFilterType;
    typedef itk::CastImageFilter<ImageType, ImageType> CastFilterType;

    itk::TransformFileReaderTemplate<double>::Pointer transformReader = itk::TransformFileReaderTemplate<double>::New();
    transformReader->SetFileName(argv[1]);

    itk::TransformFactoryBase::RegisterDefaultTransforms();

    typedef itk::ThinPlateSplineKernelTransform<double,ImageDimension> ThinPlateSplineKernelTransformType;
    typedef itk::BSplineTransform<double, ImageDimension> BSplineTransformType;
    itk::TransformFactory<ThinPlateSplineKernelTransformType>::RegisterTransform();

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
        std::cerr << "Exception thrown " << std::endl;
        std::cerr << excp << std::endl;
        return EXIT_FAILURE;
        }

    ImageType::ConstPointer movingImage = reader->GetOutput();

    CompositeTransformType::Pointer transform = CompositeTransformType::New();
    // itk::TransformFileReader::TransformListType transformList = 
    //     *(transformReader->GetTransformList());

    // itk::TransformFileReaderTemplate<double>::TransformListType::iterator iterator;
    // for(iterator = transformList.begin(); iterator != transformList.end(); ++iterator)
    // {
    //     transform->AddTransform(*iterator);
    // }

    typedef itk::TransformFileReader::TransformListType * TransformListType;
    TransformListType transforms = transformReader->GetTransformList();
    std::cout << "Number of transforms = " << transforms->size() << std::endl;

    itk::TransformFileReader::TransformListType::const_iterator it = transforms->begin();
    
    ThinPlateSplineKernelTransformType::Pointer thinPlate_read;
    if(!strcmp((*it)->GetNameOfClass(),"ThinPlateSplineKernelTransform"))
        {
        thinPlate_read = static_cast<ThinPlateSplineKernelTransformType*>((*it).GetPointer());
        thinPlate_read->Print(std::cout);
        }

    it++;

    BSplineTransformType::Pointer bspline_read;
    if(!strcmp((*it)->GetNameOfClass(),"BSplineTransform"))
        {
        bspline_read = static_cast<BSplineTransformType*>((*it).GetPointer());
        bspline_read->Print(std::cout);
        }

    transform->AddTransform(thinPlate_read);
    transform->AddTransform(bspline_read);

    // std::cout << *(transformReader->GetTransformList()->begin()) << std::endl;

    // std::ifstream parametersFile;
    // parametersFile.open(argv[1]);
    // parametersFile >> parameters;
    // parametersFile.close();

    // transform->SetParameters(parameters);

    ResampleFilterType::Pointer resample = ResampleFilterType::New();
    resample->SetTransform(transform);
    resample->SetInput(movingImage);


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
        std::cerr << "Image writer exception caught!" << std::endl;
        std::cerr << err << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Finished Applying Transformation" << std::endl << std::endl;

    return EXIT_SUCCESS;
}