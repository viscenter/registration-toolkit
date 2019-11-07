//
// Created by Anthony Tamasi on 2019-10-31.
//

#include "rt/Data.hpp"

using namespace Data;

Data::Pointer Data::Load(std::string path)
{

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