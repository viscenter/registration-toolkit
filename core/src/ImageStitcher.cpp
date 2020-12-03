// clang-format off
/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                          License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/
// clang-format on

#include "rt/ImageStitcher.hpp"

#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/features2d.hpp>

using namespace rt;
namespace cvd = cv::detail;

using ImgList = std::vector<cv::UMat>;
using FeatureList = std::vector<cvd::ImageFeatures>;
using LdmPairList = std::vector<ImageStitcher::LandmarkPair>;

static ImgList ScaleImages(
    const ImgList& imgs,
    double scale,
    cv::InterpolationFlags interp = cv::INTER_LINEAR)
{
    ImgList scaled;
    scaled.reserve(imgs.size());
    for (const auto& i : imgs) {
        cv::UMat s;
        cv::resize(i, s, cv::Size(), scale, scale, interp);
        scaled.emplace_back(s);
    }
    return scaled;
}

static FeatureList DetectFeatures(
    const ImgList& imgs,
    const ImgList& masks,
    double workScale,
    const cv::Ptr<cv::Feature2D>& finder);
static cvd::ImageFeatures LandmarkToFeature(
    const Landmark& l, const cv::Ptr<cv::Feature2D>& finder);
static FeatureList LandmarkContainerToFeatureList(
    const LandmarkContainer& lc, const cv::Ptr<cv::Feature2D>& finder);

int FindFeatureIndex(size_t idx, const FeatureList& fl);

int search_(cvd::ImageFeatures& features, Landmark& point);
void filter_matched_features_(
    cvd::ImageFeatures& features,
    LandmarkContainer& points1,
    LandmarkContainer& points2);
void reduce_img_points_(
    const double& work_scale,
    LandmarkContainer& features1,
    LandmarkContainer& features2);

void ImageStitcher::setImages(const std::vector<cv::Mat>& images)
{
    input_ = images;
}

void ImageStitcher::setLandmarkMode(LandmarkMode option) { ldmMode_ = option; }

cvd::MatchesInfo ImageStitcher::compute_homography_(
    cvd::MatchesInfo matchesInfo)
{
    // Construct point-point correspondences for transform estimation
    cv::Mat srcPoints(1, static_cast<int>(matchesInfo.matches.size()), CV_32FC2);
    cv::Mat dstPoints(1, static_cast<int>(matchesInfo.matches.size()), CV_32FC2);
    for (size_t i = 0; i < matchesInfo.matches.size(); ++i)
    {
        const cv::DMatch &m = matchesInfo.matches[i];
        srcPoints.at<cv::Point2f>(0, static_cast<int>(i)) =
            features[matchesInfo.src_img_idx].keypoints[m.queryIdx].pt;
        dstPoints.at<cv::Point2f>(0, static_cast<int>(i)) =
            features[matchesInfo.dst_img_idx].keypoints[m.trainIdx].pt;
    }
    matchesInfo.H = estimateAffinePartial2D(srcPoints, dstPoints, matchesInfo.inliers_mask);

    if (matchesInfo.H.empty()) {
        // could not find transformation
        matchesInfo.confidence = 0;
        matchesInfo.num_inliers = 0;
        return matchesInfo;
    }

    // Find number of inliers
    matchesInfo.num_inliers = 0;
    for (size_t i = 0; i < matchesInfo.inliers_mask.size(); ++i) {
        if (matchesInfo.inliers_mask[i]) {
            matchesInfo.num_inliers++;
        }
    }

    // These coeffs are from paper M. Brown and D. Lowe. "Automatic Panoramic
    // Image Stitching using Invariant Features"
    matchesInfo.confidence =
            matchesInfo.num_inliers / (8 + 0.3 * matchesInfo.matches.size());

    // extend H to represent linear transformation in homogeneous coordinates
    matchesInfo.H.push_back(cv::Mat::zeros(1, 3, CV_64F));
    matchesInfo.H.at<double>(2, 2) = 1;

    return matchesInfo;
}

int search_(cvd::ImageFeatures& features, Landmark& point)
{
    for (int j = 0; j < features.keypoints.size(); j++) {
        if (std::abs(features.keypoints[j].pt.x - point.first) < 0.001) {
            if (std::abs(features.keypoints[j].pt.y - point.second) < 0.001) {
                return j;
            }
        }
    }
    return -1;
}

void filter_matched_features_(
    cvd::ImageFeatures& features,
    LandmarkContainer& points1,
    LandmarkContainer& points2)
{
    for(int i = 0; i < points1.size(); i++){
        // Implement search so it finds the index for the point
        // Returns -1 if the points cannot be found
        int index = search_(features, points1[i]);
        if(index < 0){
            // Check to see if this is the right way to do this
            // Want to remove the points that are filtered out for points1
            // Want to take out the match for the point that isn't there anymore
            points1.erase(points1.begin() + i);
            points2.erase(points2.begin() + i);
            i--;
        }
    }
}

void reduce_img_points_(
    const double& work_scale,
    LandmarkContainer& features1,
    LandmarkContainer& features2)
{
    for(int i = 0; i < features1.size(); i++){
        features1[i].first = features1[i].first * work_scale;
        features1[i].second = features1[i].second * work_scale;
        features2[i].first = features1[i].first * work_scale;
        features2[i].second = features2[i].second * work_scale;
    }
}

// This will probably need to be changed to account for having more than 2 images
void ImageStitcher::setLandmarks(std::vector<LandmarkPair> ldms){
    landmarks_ = ldms;
}

void ImageStitcher::find_matches_(double confThresh, std::vector<cv::UMat>& seamEstImgs, std::vector<cv::Size>& fullImgSizes) {
    auto featuresMatcher =
        cv::makePtr<cvd::AffineBestOf2NearestMatcher>(false, false);
    cv::UMat matchingMask;

    (*featuresMatcher)(features, allPairwiseMatches_, matchingMask);
    featuresMatcher->collectGarbage();

    // This is for option 1 with placing the matches before filtering the images
    if (ldmMode_ == LandmarkMode::ManualPreMatch && !landmarks_.empty()) {
        for(int i = 0; i < landmarks_.size(); i++){
            insert_user_matches_(landmarks_[i]);
        }
    }
    // Leave only images we are sure are from the same panorama
    auto indices_ = cvd::leaveBiggestComponent(
        features, allPairwiseMatches_, (float)confThresh);
    std::vector<cv::UMat> seamEstImgsSubset;
    std::vector<cv::UMat> imgsSubset;
    std::vector<cv::Size> fullImgSizesSubset;
    for (size_t i = 0; i < indices_.size(); ++i) {
        imgsSubset.push_back(imgs_[indices_[i]]);
        seamEstImgsSubset.push_back(seamEstImgs[indices_[i]]);
        fullImgSizesSubset.push_back(fullImgSizes[indices_[i]]);
    }
    seamEstImgs = seamEstImgsSubset;
    imgs_ = imgsSubset;
    fullImgSizes = fullImgSizesSubset;

}

std::vector<cvd::CameraParams> ImageStitcher::estimate_camera_params_(
    double confThresh, float& warpedImageScale)
{
    //////////////////////////////////
    ///// Estimate camera params /////
    //////////////////////////////////
    // estimate homography in global frame
    cv::Ptr<cvd::BundleAdjusterBase> bundleAdjuster =
        cv::makePtr<cvd::BundleAdjusterAffinePartial>();
    cv::Ptr<cvd::Estimator> estimator =
        cv::makePtr<cvd::AffineBasedEstimator>();
    std::vector<cvd::CameraParams> cameras;
    if (!(*estimator)(features, allPairwiseMatches_, cameras)) {
        throw std::runtime_error("Could not estimate camera homography");
    }

    for (size_t i = 0; i < cameras.size(); ++i) {
        cv::Mat R;
        cameras[i].R.convertTo(R, CV_32F);
        cameras[i].R = R;
    }

    bundleAdjuster->setConfThresh(confThresh);
    if (!(*bundleAdjuster)(features, allPairwiseMatches_, cameras)) {
        throw std::runtime_error("Failed bundle adjustment");
    }

    // Find median focal length and use it as final image scale
    std::vector<double> focals;
    for (size_t i = 0; i < cameras.size(); ++i) {
        focals.push_back(cameras[i].focal);
    }

    std::sort(focals.begin(), focals.end());
    bool do_wave_correct_{false};
    cvd::WaveCorrectKind wave_correct_kind_{
        cvd::WaveCorrectKind::WAVE_CORRECT_HORIZ};
    if (focals.size() % 2 == 1) {
        warpedImageScale = static_cast<float>(focals[focals.size() / 2]);
    }
    else {
        warpedImageScale =
                static_cast<float>(
                        focals[focals.size() / 2 - 1] + focals[focals.size() / 2]) *
                0.5f;
    }

    if (do_wave_correct_) {
        std::vector<cv::Mat> rmats;
        for (size_t i = 0; i < cameras.size(); ++i)
            rmats.push_back(cameras[i].R.clone());
        cvd::waveCorrect(rmats, wave_correct_kind_);
        for (size_t i = 0; i < cameras.size(); ++i)
            cameras[i].R = rmats[i];
    }
    return cameras;
}

cv::Mat ImageStitcher::compose_pano_(
    double seamWorkAspect,
    float warpedImageScale,
    double workScale,
    std::vector<cv::UMat>& seamEstImgs,
    std::vector<cvd::CameraParams>& cameras,
    std::vector<cv::Size>& fullImgSizes)
{
    //////////////////////
    //// compose pano ////
    //////////////////////
    cv::Mat pano;

    double composeResol{cv::Stitcher::ORIG_RESOL};
    std::vector<cv::Point> corners(imgs_.size());
    std::vector<cv::UMat> masksWarped(imgs_.size());
    std::vector<cv::UMat> imagesWarped(imgs_.size());
    std::vector<cv::Size> sizes(imgs_.size());
    std::vector<cv::UMat> masks(imgs_.size());

    // Prepare image masks
    for (size_t i = 0; i < imgs_.size(); ++i) {
        masks[i].create(seamEstImgs[i].size(), CV_8U);
        masks[i].setTo(cv::Scalar::all(255));
    }

    // Warp images and their masks
    cv::Ptr<cv::WarperCreator> warper = cv::makePtr<cv::AffineWarper>();
    cv::Ptr<cvd::RotationWarper> w =
        (warper)->create(float(warpedImageScale * seamWorkAspect));
    cv::InterpolationFlags interpFlags{cv::INTER_LINEAR};
    for (size_t i = 0; i < imgs_.size(); ++i) {
        cv::Mat_<float> K;
        cameras[i].K().convertTo(K, CV_32F);
        K(0, 0) *= (float)seamWorkAspect;
        K(0, 2) *= (float)seamWorkAspect;
        K(1, 1) *= (float)seamWorkAspect;
        K(1, 2) *= (float)seamWorkAspect;

        corners[i] = w->warp(
                seamEstImgs[i], K, cameras[i].R, interpFlags,
                cv::BORDER_REFLECT, imagesWarped[i]);
        sizes[i] = imagesWarped[i].size();

        w->warp(
                masks[i], K, cameras[i].R, cv::INTER_NEAREST, cv::BORDER_CONSTANT,
                masksWarped[i]);
    }

    // Compensate exposure before finding seams
    cv::Ptr<cvd::ExposureCompensator> exposureComp =
        cv::makePtr<cvd::NoExposureCompensator>();
    exposureComp->feed(corners, imagesWarped, masksWarped);
    for (size_t i = 0; i < imgs_.size(); ++i) {
        exposureComp->apply(
                int(i), corners[i], imagesWarped[i], masksWarped[i]);
    }

    // Find seams
    cv::Ptr<cvd::SeamFinder> seamFinder = cv::makePtr<cvd::GraphCutSeamFinder>(
        cvd::GraphCutSeamFinderBase::COST_COLOR);
    std::vector<cv::UMat> imagesWarpedF(imgs_.size());
    for (size_t i = 0; i < imgs_.size(); ++i) {
        imagesWarped[i].convertTo(imagesWarpedF[i], CV_32F);
    }
    seamFinder->find(imagesWarpedF, corners, masksWarped);

    // Release unused memory
    seamEstImgs.clear();
    imagesWarped.clear();
    imagesWarpedF.clear();
    masks.clear();

    // Compositing
    cv::UMat imgWarped, imgWarpedS;
    cv::UMat dilatedMask, seamMask, mask, maskWarped;

    // double compose_seam_aspect = 1;
    double composeWorkAspect = 1;
    bool isBlenderPrepared = false;

    double composeScale = 1;
    bool isComposeScaleSet = false;

    std::vector<cvd::CameraParams> camerasScaled(cameras);

    cv::UMat fullImg, img;
    cv::Ptr<cvd::Blender> blender = cv::makePtr<cvd::MultiBandBlender>(false);

    for (size_t img_idx = 0; img_idx < imgs_.size(); ++img_idx) {
        // Read image and resize it if necessary
        fullImg = imgs_[img_idx];
        if (!isComposeScaleSet) {
            if (composeResol > 0) {
                composeScale = std::min(
                        1.0,
                        std::sqrt(composeResol * 1e6 / fullImg.size().area()));
            }
            isComposeScaleSet = true;

            // Compute relative scales
            // compose_seam_aspect = composeScale / seam_scale_;
            composeWorkAspect = composeScale / workScale;

            // Update warped image scale
            float warpScale =
                    static_cast<float>(warpedImageScale * composeWorkAspect);
            w = warper->create(warpScale);

            // Update corners and sizes
            for (size_t i = 0; i < imgs_.size(); ++i) {
                // Update intrinsics
                camerasScaled[i].ppx *= composeWorkAspect;
                camerasScaled[i].ppy *= composeWorkAspect;
                camerasScaled[i].focal *= composeWorkAspect;

                // Update corner and size
                cv::Size sz = fullImgSizes[i];
                if (std::abs(composeScale - 1) > 1e-1) {
                    sz.width =
                            cvRound(fullImgSizes[i].width * composeScale);
                    sz.height =
                            cvRound(fullImgSizes[i].height * composeScale);
                }

                cv::UMat K;
                camerasScaled[i].K().convertTo(K, CV_32F);
                cv::Rect roi = w->warpRoi(sz, K, camerasScaled[i].R);
                corners[i] = roi.tl();
                sizes[i] = roi.size();
            }
        }
        if (std::abs(composeScale - 1) > 1e-1) {
            resize(
                    fullImg, img, cv::Size(), composeScale, composeScale,
                    cv::INTER_LINEAR);
        } else {
            img = fullImg;
        }
        fullImg.release();
        cv::Size imgSize = img.size();

        cv::UMat K;
        camerasScaled[img_idx].K().convertTo(K, CV_32F);

        // Warp the current image
        w->warp(
                img, K, cameras[img_idx].R, interpFlags, cv::BORDER_REFLECT,
                imgWarped);

        // Warp the current image mask
        mask.create(imgSize, CV_8U);
        mask.setTo(cv::Scalar::all(255));
        w->warp(
                mask, K, cameras[img_idx].R, cv::INTER_NEAREST,
                cv::BORDER_CONSTANT, maskWarped);

        // Compensate exposure
        exposureComp->apply(
                (int)img_idx, corners[img_idx], imgWarped, maskWarped);

        imgWarped.convertTo(imgWarpedS, CV_16S);
        imgWarped.release();
        img.release();
        mask.release();

        // Make sure seam mask has proper size
        cv::dilate(masksWarped[img_idx], dilatedMask, cv::UMat());
        cv::resize(
                dilatedMask, seamMask, maskWarped.size(), 0, 0,
                cv::INTER_LINEAR);

        cv::bitwise_and(seamMask, maskWarped, maskWarped);

        // Blender
        if (!isBlenderPrepared) {
            blender->prepare(corners, sizes);
            isBlenderPrepared = true;
        }

        // Blend the current image
        blender->feed(imgWarpedS, maskWarped, corners[img_idx]);
    }

    cv::UMat result;
    cv::UMat resultMask;
    blender->blend(result, resultMask);

    // Preliminary result is in CV_16SC3 format, but all values are in [0,255]
    // range, so convert it to avoid user confusing
    result.convertTo(pano, CV_8U);
    return pano;
}

int ImageStitcher::search_matches_(int src, int dst){
    // Need to find more efficient way to search
    for(int i = 0; i < allPairwiseMatches_.size(); i++){
        if (src == allPairwiseMatches_[i].src_img_idx) {
            if (dst == allPairwiseMatches_[i].dst_img_idx) {
                return i;
            }
        }
    }
    return -1;
}

void ImageStitcher::create_matches_() {
    for(int i = 0; i < imgs_.size(); i++){
        for(int j = 0; j < imgs_.size(); j++){
            cvd::MatchesInfo match;
            if( i == j){
                match.src_img_idx = -1;
                match.dst_img_idx = -1;
            }
            else {
                match.src_img_idx = i;
                match.dst_img_idx = j;
            }
            allPairwiseMatches_.push_back(match);
        }
    }
}

// Need to give a flag that will print the images within a certain folder
void ImageStitcher::printFeatures(std::string filePath){
    cv::UMat output;
    for (int i = 0; i < features.size(); i++) {
        cv::drawKeypoints(
            imgs_[features[i].img_idx], features[i].keypoints, output,
            cv::Scalar(0, 255, 0), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        cv::imwrite(filePath + "features_" + std::to_string(i) + ".jpg", output);
    }
}

// Need to give a flag that will print the images within a certain folder
void ImageStitcher::printMatches(std::string filePath){
    cv::UMat matchesOutput;
    for(int i = 0; i < allPairwiseMatches_.size(); i++) {
        if(allPairwiseMatches_[i].src_img_idx != -1 && allPairwiseMatches_[i].dst_img_idx != -1 && allPairwiseMatches_[i].src_img_idx != allPairwiseMatches_[i].dst_img_idx) {
            if (!allPairwiseMatches_[i].matches.empty()) {
                int src = allPairwiseMatches_[i].src_img_idx;
                int dest = allPairwiseMatches_[i].dst_img_idx;
                cv::drawMatches(
                    imgs_[src], features[src].keypoints, imgs_[dest],
                    features[dest].keypoints, allPairwiseMatches_[i].matches,
                    matchesOutput, cv::Scalar(0, 255, 0),
                    cv::Scalar(0, 255, 0));
                cv::imwrite(filePath + "matches_" + std::to_string(i) + ".jpg",
                            matchesOutput);
            }
        }
    }
}

// The implementation of this function is a modified version of
// cv::Stitcher::stitch()
cv::Mat ImageStitcher::compute()
{
    // Sanity check
    if (input_.size() < 2) {
        throw std::runtime_error("Not enough images to perform stitching");
    }

    // Currently conf_thres = 1 if using automatic stitching
    // and conf_thres = 0.1 if using user landmarks
    //double confThresh{1};
    double confThresh{0.1};

    // Calculate working scale for registration and seam finding
    double regResol{-0.6};
    double seamEstResol{0.1};
    double workScale{1};
    double seamScale{1};
    double seamWorkAspect{1};
    auto sizeEst = input_[0].size();
    if (regResol > 0) {
        workScale = std::min(1.0, std::sqrt(regResol * 1e6 / sizeEst.area()));
    }
    seamScale = std::min(1.0, std::sqrt(seamEstResol * 1e6 / sizeEst.area()));
    seamWorkAspect = seamScale / workScale;

    // Convert images to umats
    ImgList umats;
    for (const auto& i : input_) {
        umats.emplace_back(i.getUMat(cv::ACCESS_READ));
    }
    ImgList masks;
    for (const auto& i : masks_) {
        masks.emplace_back(i.getUMat(cv::ACCESS_READ));
    }

    //////////////////////
    //// match images ////
    //////////////////////
    // Setup feature finder
    featureFinder_ = cv::ORB::create();

    // Always generate landmarks unless we're in full manual mode
    FeatureList features;
    if (ldmMode_ != LandmarkMode::Manual) {
        features = DetectFeatures(umats, masks, workScale, featureFinder_);
    }

    // If manual or if pre-filter, insert user landmarks
    if (ldmMode_ == LandmarkMode::Manual or
        ldmMode_ == LandmarkMode::ManualPreMatch) {
        for (const auto& l : landmarks_) {
            insert_user_matches_(l);
        }
    }

    // Match landmarks
    find_matches_(confThresh, seamEstImgs, fullImgSizes);

    // If post-filter, insert landmarks
    if (ldmMode_ == LandmarkMode::ManualPostMatch) {
        for (const auto& l : landmarks_) {
            insert_user_matches_(l);
        }
    }

    //////////////////////////////////
    ///// Estimate camera params /////
    //////////////////////////////////
    float warpedImageScale;
    auto cameras = estimate_camera_params_(confThresh, warpedImageScale);

    //////////////////////
    //// compose pano ////
    //////////////////////
    result_ = compose_pano_(
        seamWorkAspect, warpedImageScale, seamScale, seamEstImgs, cameras,
        fullImgSizes);

    return result_;
}

static FeatureList DetectFeatures(
    const ImgList& imgs,
    const ImgList& masks,
    double workScale,
    const cv::Ptr<cv::Feature2D>& finder)
{

    ImgList scaledImgs;
    ImgList scaledMasks;

    // Scale images if needed
    if (workScale == 1.0) {
        scaledImgs = imgs;
        scaledMasks = masks;
    } else {
        scaledImgs = ScaleImages(imgs, workScale);
        scaledMasks = ScaleImages(masks, workScale, cv::INTER_LINEAR);
    }

    // Compute features using ORB
    std::vector<cvd::ImageFeatures> features(imgs.size());
    cvd::computeImageFeatures(finder, scaledImgs, features, scaledMasks);

    return features;
}

int FindFeatureIndex(size_t idx, const FeatureList& fl)
{
    int it{0};
    for (const auto& f : fl) {
        if (it == f.img_idx) {
            return it;
        }
        it++;
    }
    return -1;
}

inline int InsertLandmarks(
    int lpIdx,
    const LandmarkContainer& lpLdms,
    FeatureList& fl,
    const ImgList& imgs)
{
    // Find src feature index
    auto idx = FindFeatureIndex(lpIdx, fl);

    // No feature found, so add it
    int size{0};
    if (idx < 0) {
        cvd::ImageFeatures f;
        f.img_idx = lpIdx;
        f.img_size = imgs[lpIdx].size();
        for (const auto& ldm : lpLdms) {
            // TODO: Size/Diameter is a magic number
            f.keypoints.emplace_back(ldm[0], ldm[1], 31);
        }
    }

    // Feature list already exists, so append
    else {
        size = fl[idx].keypoints.size();
        for (const auto& ldm : lpLdms) {
            fl[idx].keypoints.emplace_back(ldm[0], ldm[1], 31);
        }
    }

    return size;
}

static FeatureList InsertUserMatches(
    const LdmPairList& lpl, FeatureList fl, const ImgList& imgs)
{
    // For each LandmarkPair
    for (const auto& lp : lpl) {
        auto srcSize = InsertLandmarks(lp.srcIdx, lp.srcLdms, fl, imgs);
        auto dstSize = InsertLandmarks(lp.dstIdx, lp.dstLdms, fl, imgs);

        // Populate the matches and put them into pairwise matches
        int srcDstIdx = search_matches_(ldmPair.srcIdx, ldmPair.dstIdx);
        cvd::MatchesInfo matches1 = allPairwiseMatches_[srcDstIdx];
        for (int i = 0; i < ldmPair.srcLdms.size(); i++) {
            matches1.matches.emplace_back(srcSize + i, dstSize + i, 0);
        }
        matches1 = compute_homography_(matches1);
        allPairwiseMatches_[srcDstIdx] = matches1;

        int dstSrcIdx = search_matches_(ldmPair.dstIdx, ldmPair.srcIdx);
        cvd::MatchesInfo matches2 = allPairwiseMatches_[dstSrcIdx];
        for (int i = 0; i < matches1.matches.size(); i++) {
            matches2.matches.emplace_back(
                matches1.matches[i].trainIdx, matches1.matches[i].queryIdx, 0);
        }
        matches2.inliers_mask = matches1.inliers_mask;
        matches2.confidence = matches1.confidence;
        matches2.num_inliers = matches1.num_inliers;
        matches2.H = matches1.H.inv();
        allPairwiseMatches_[dstSrcIdx] = matches2;
    }
}