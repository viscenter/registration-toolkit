#pragma once

namespace rt
{
class LandmarkRegistration {
public:
    LandmarkRegistration(
        const Image::Pointer fixedImage, 
        const Image::Pointer movingImage,
        const std::ifstream& pointsFile)
        : fixedImage_(fixedImage), movingImage_(movingImage), pointsFile_(pointsFile)
    {
    }

    ResampleFilter::Pointer resampleImages();

private:
    void read_points_file_();

    /** Gets the Affine Transform from the given fixed and moving landmarks */ 
    void generate_affine_transform_();
    ResampleFilter::Pointer resample_();

    /** Images we are trying to register together */
    Image::Pointer fixedImage_;
    Image::Pointer movingImage_;

    /** File stream that contains points */
    std::ifstream pointsFile_;

    LandmarkContainer fixedLandmarks_;
    LandmarkContainer movingLandmarks_;

    /** Transform applied to the resampling image */
    AffineTransform::Pointer landmarkTransform_;
}
}