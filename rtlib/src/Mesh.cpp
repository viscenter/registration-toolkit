//
// Created by Anthony Tamasi on 2020-02-04.
//

#include "rt/Mesh.hpp"

#include <itkOpenCVImageBridge.h>

#include "rt/io/OBJReader.hpp"
#include "rt/types/ITKMesh.hpp"

using namespace rt;
using OCVBridge = itk::OpenCVImageBridge;
namespace fs = boost::filesystem;


rt::Mesh::Mesh(const fs::path& path)
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
    }
}


