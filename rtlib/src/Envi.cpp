//
// Created by Anthony Tamasi on 2020-02-04.
//

#include "rt/Envi.hpp"

using namespace rt;


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

