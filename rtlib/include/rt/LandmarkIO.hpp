#pragma once

using LandmarkContainer = LandmarkTransformInitializer::LandmarkPointContainer;

namespace rt
{
class LandmarkIO {
public:
    LandmarkIO(const boost::filesystem::path& landmarksPath) 
        : landmarksPath_(landmarksPath) {
            fixedImage_ = NULL; 
            movingImage_ = NULL;
        }

        void setFixedImage(const Image::Pointer image) { fixedImage_ = image; }
        void setMovingImage(const Image::Pointer image) { movingImage_ = image; }
        void setLandmarksPath(const boost::filesystem::path& path) { landmarksPath_ = path; }
        void read();

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

};
}