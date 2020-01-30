//
// Created by Anthony Tamasi on 2019-10-31.
//

#include <itkOpenCVImageBridge.h>

#include "rt/Data.hpp"
#include "rt/io/OBJReader.hpp"

namespace et = envitools;
using namespace Data;

// IO
using OCVBridge = itk::OpenCVImageBridge;


Data::Pointer Data::Load(const fs::path& path)
{
    //HOW TO GET METADATA AKA FILE TYPE? Could just read the 'metadata.json' file from the current directory right? Since each
    //input data thing in the RegPkg is a sub-folder that contains only the data and then the metadata file.
    //Read in metadata file

    //Select type of object based on this file's type
    switch(metadata.type){
        case "image":
            return std::make_shared<Image>(path); //SHOULD THESE BE SHARED POINTERS INSTEAD OF "new"? e.g. return std::make_unique<Image>(path);?
        case "envi":
            return std::make_shared<ENVI>(path);
        case "mesh":
            return std::make_shared<Mesh>(path);
    }
}

Data::Image(const fs::path& path) { img_ = cv::imread(path.string()); }

Data::ENVI(const fs::path& path)
{
    //Read in ENVI file from the file path, set the object to the envi_ private member
    envi_ = et::ENVI envi(path); //or does it need to be envitools::ENVI envi(path);
    //or is it et::ENVI envi_(path);

}

cv::Mat Data::ENVI::getImage(int idx)
{
    //Get image of particular spectral band
    cv::Mat img = envi_.getBand(idx);

    //envi_.closeFile();

    return img;
}

Data::Mesh(const fs::path& path)
{
    //Read in mesh along with its texture
    io::OBJReader reader;
    reader.setPath(path);

    try {
        origMesh_ = reader.read();
        cvFixedImage_ = reader.getTextureMat();
        fixedImage_ = OCVBridge::CVMatToITKImage<Image8UC3>(cvFixedImage_);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}