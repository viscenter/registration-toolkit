//
// Created by Anthony Tamasi on 2019-10-31.
//

#include <itkOpenCVImageBridge.h>

#include "rt/Data.hpp"
#include "rt/io/OBJReader.hpp"

namespace et = envitools;
using namespace rt;

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
