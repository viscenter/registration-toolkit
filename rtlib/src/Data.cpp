//
// Created by Anthony Tamasi on 2019-10-31.
//

#include <itkOpenCVImageBridge.h>
#include <rt/Envi.hpp>
#include <rt/Image.hpp>
#include <rt/Mesh.hpp>

#include "rt/Data.hpp"
#include "rt/io/OBJReader.hpp"

namespace et = envitools;
namespace fs = boost::filesystem;
using json = nlohmann::json;
using namespace rt;

// IO
using OCVBridge = itk::OpenCVImageBridge;

enum class Type { Image, Envi, Mesh };

Data::Pointer Data::Load(const fs::path& path)
{
    fs::path metadata_file;

    //Find this file path's metadata file
    for (fs::directory_entry& entry : fs::directory_iterator(path)){
        if(entry.path().stem() == "metadata" && entry.path().extension() != "json") {
            metadata_file = entry.path();
        }
    }

    //Read "spatial object type" of the metadata file into JSON object
    std::ifstream input_stream(metadata_file.string());
    json j;
    input_stream >> j;

    Type metadata_type = j["type"];


    //Select type of object based on this file's type
    switch(metadata_type){
        case Type::Image:
            return std::make_shared<rt::Image>(path);
        case Type::Envi:
            return std::make_shared<rt::ENVI>(path);
        case Type::Mesh:
            return std::make_shared<rt::Mesh>(path);
    }
}
