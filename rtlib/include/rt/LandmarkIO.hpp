#pragma once

namespace rt
{
class LandmarkIO {
public:
    LandmarkIO(
        const Image::Pointer fixedImage, const Image::Pointer movingImage, const boost::filesystem::path& landmarksPath) 
        : fixedImage_(fixedImage), movingImage_(movingImage), landmarksPath_(landmarksPath);

        void setFixedImage(const Image::Pointer image) { fixedImage_ = image; }
        void setMovingImage(const Image::Pointer image) { movingImage_ = image; }
        void setLandmarksPath(const boost::filesystem::path& path) { landmarksPath_ = path; }
        void computeLandmarks();

        LandmarkContainer getFixedLandmarks() { return fixedLandmarks_; }
        LandmarkContainer getMovingLandmarks() { return movingLandmarks_; }

private:
    void clear_();
    void read_landmarks_file_();

    Image::Pointer fixedImage_;
    Image::Pointer movingImage_;
    boost::filesystem::path landmarksPath_;
    LandmarkContainer fixedLandmarks_;
    LandmarkContainer movingLandmarks_;

}
}