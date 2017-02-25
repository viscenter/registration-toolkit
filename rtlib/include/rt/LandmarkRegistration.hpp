#pragma once
#include "LandmarkIO.hpp"

namespace rt
{
class LandmarkRegistration {
public:
    LandmarkRegistration(
        const Image::Pointer fixedImage, 
        const Image::Pointer movingImage,
        const boost::filesystem::path& landmarksPath)
        : io_(fixedImage, movingImage, landmarksPath)
    {
    }

    AffineTransform::Pointer getTransform();

private:
    /** Gets the Affine Transform from the given fixed and moving landmarks */ 
    AffineTransform::Pointer generate_affine_transform_();
    ResampleFilter::Pointer resample_();

    LandmarkIO io_;

    /** Transform applied to the resampling image */
}
}