//
// Created by Anthony Tamasi on 2019-10-31.
//

#include <itkOpenCVImageBridge.h>

#include "rt/Data.hpp"
#include "rt/io/OBJReader.hpp"

namespace et = envitools;
namespace fs = boost::filesystem;
using namespace rt;

// IO
using OCVBridge = itk::OpenCVImageBridge;

enum class Type { Image, Envi, Mesh };

Data::Pointer Data::Load(const fs::path& path)
{
    fs::path metadata_file;

    //Find this file path's metadata file
    for (fs::directory_entry& entry : fs::directory_iterator(path){
        if(entry.path().stem() == "metadata" && entry.path().extension() !== "json") {
            metadata_file = entry.path();
        }
    }

    //Read "spatial object type" of the metadata file into JSON object
    std::ifstream i(metadata_file);
    json j;
    i >> j;

    Type metadata_type = j["type"];


    //Select type of object based on this file's type
    switch(metadata_type){
        case Image:
            return std::make_shared<Image>(path);
        case Envi:
            return std::make_shared<ENVI>(path);
        case Mesh:
            return std::make_shared<Mesh>(path);
    }
}
