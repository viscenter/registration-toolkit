//
// Created by Anthony Tamasi on 2019-10-31.
//

#include <itkOpenCVImageBridge.h>

#include "rt/Data.hpp"
#include "rt/io/OBJReader.hpp"


using namespace Data;

// IO
using OCVBridge = itk::OpenCVImageBridge;

Data::Pointer Data::Load(std::string path)
{
    //HOW TO GET METADATA AKA FILE TYPE? Could just read the 'metadata.json' file from the current directory right? Since each
    //input data thing in the RegPkg is a sub-folder that contains only the data and then the metadata file.
    //Read in metadata file

    //Select type of object based on this file's type
    switch(metadata.type){
        case "image":
            return new Image(path); //SHOULD THESE BE SHARED POINTERS INSTEAD OF "new"? e.g. return std::make_unique<Image>(path);?
        case "envi":
            return new ENVI(path);
        case "mesh":
            return new Mesh(path);
    }
}


Data::Image(std::string path)
{
    img_ = cv::imread(path.string());
}


Data::ENVI(std::string path)
{
    //Read in ENVI file from the file path

    //Read in all bands, each as a cv::Mat

    //Add band to the private member vector "bands_"
}

Data::Mesh(std::string path)
{
    //Read in mesh along with it's texture
    io::OBJReader reader;
    reader.setPath(fixedPath);
    //ITKMesh::Pointer origMesh;
    //cv::Mat cvFixedImage;
    try {
        origMesh_ = reader.read();
        cvFixedImage_ = reader.getTextureMat();
        fixedImage_ = OCVBridge::CVMatToITKImage<Image8UC3>(cvFixedImage_);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}