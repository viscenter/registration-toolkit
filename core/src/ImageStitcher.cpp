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

#include <algorithm>

#include "rt/ImageStitcher.hpp"

#include <opencv2/calib3d.hpp>
#include <opencv2/stitching.hpp>
#include <opencv2/features2d.hpp>

using namespace rt;
namespace cvd = cv::detail;

///// Type aliases /////
using ImgList = std::vector<cv::UMat>;
using FeatureList = std::vector<cvd::ImageFeatures>;
using MatchesList = std::vector<cvd::MatchesInfo>;
using LdmPairList = std::vector<ImageStitcher::LandmarkPair>;
using CameraList = std::vector<cvd::CameraParams>;

///// Predeclarations /////
static ImgList ScaleImages(
    const ImgList& imgs,
    float scale,
    cv::InterpolationFlags interp = cv::INTER_LINEAR);

static FeatureList DetectFeatures(
    const ImgList& imgs,
    const ImgList& masks,
    float workScale,
    const cv::Ptr<cv::Feature2D>& finder);

static void MatchFeatures(float confThresh, FeatureList& fl, MatchesList& ml);

static cvd::ImageFeatures LandmarkToFeature(
    const Landmark& l, const cv::Ptr<cv::Feature2D>& finder);

static int FindFeatureIndex(int idx, const FeatureList& fl);

static int FindMatchesIndex(int srcIdx, int dstIdx, const MatchesList& ml);

static int InsertLandmarks(
    int lpIdx,
    const LandmarkContainer& lpLdms,
    FeatureList& fl,
    const ImgList& imgs);

static void InsertUserMatches(
    const LdmPairList& lpl,
    FeatureList& fl,
    MatchesList& ml,
    const ImgList& imgs);

template <typename ObjList, typename IndexList>
ObjList KeepByIndex(const ObjList& ol, const IndexList& il);

static CameraList EstimateCameras(
    const FeatureList& fl,
    const MatchesList& ml,
    float confThresh,
    float& warpedScale);

static cv::Mat ComposePano(
    const ImgList& imgs,
    const CameraList& cams,
    float seamAspect,
    float seamScale,
    float warpScale);

static cvd::MatchesInfo ComputeHomography(
    cvd::MatchesInfo matchesInfo, const FeatureList& fl);

///// Member functions /////
void ImageStitcher::setImages(const std::vector<cv::Mat>& images)
{
    input_ = images;
}

void ImageStitcher::setLandmarkMode(LandmarkMode option) { ldmMode_ = option; }

// This will probably need to be changed to account for having more than 2
// images
void ImageStitcher::setLandmarks(const std::vector<LandmarkPair>& ldms)
{
    landmarks_ = ldms;
}

// The implementation of this function is a modified version of
// cv::Stitcher::stitch()
cv::Mat ImageStitcher::compute()
{
    // Sanity check
    if (input_.size() < 2) {
        throw std::runtime_error("Not enough images to perform stitching");
    }

    // Confidence threshold
    float confThresh{0.3F};

    // Calculate working scale for registration and seam finding
    float regResol{-0.6F};
    float seamEstResol{0.1F};
    float workScale{1};
    float seamScale{1};
    float seamWorkAspect{1};
    auto area = static_cast<float>(input_[0].size().area());
    if (regResol > 0) {
        workScale = std::min(1.F, std::sqrt(regResol * 1e6F / area));
    }
    seamScale = std::min(1.F, std::sqrt(seamEstResol * 1e6F / area));
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
    // Setup features and matches containers
    FeatureList features;
    MatchesList matches;

    // Always generate auto-landmarks unless we're in full manual mode
    if (ldmMode_ != LandmarkMode::Manual) {
        auto featureFinder = cv::ORB::create();
        features = DetectFeatures(umats, masks, workScale, featureFinder);
    }

    // If pre-matching, insert user landmarks
    // Note: Matches will get ignored by matcher, so prob not great?
    if (ldmMode_ == LandmarkMode::ManualPreMatch) {
        InsertUserMatches(landmarks_, features, matches, umats);
    }

    // Perform matching
    // TODO: If manual, features will be empty
    MatchFeatures(confThresh, features, matches);

    // If manual or post-matching, insert user landmarks and matches
    if (ldmMode_ == LandmarkMode::Manual or
        ldmMode_ == LandmarkMode::ManualPostMatch) {
        InsertUserMatches(landmarks_, features, matches, umats);
    }

    // If fallback, insert landmarks for the images that got removed
    // TODO: Fallback matches should be added after biggest component filtering
    // but src and dst indices won't match
    if (ldmMode_ == LandmarkMode::ManualFallback) {
        std::vector<LandmarkPair> fallbackLdms;
        for (const auto& l : landmarks_) {
            auto idx = FindMatchesIndex(l.srcIdx, l.dstIdx, matches);
            // Match will be below BC threshold, so add as a landmark
            if (matches[idx].confidence < confThresh) {
                fallbackLdms.emplace_back(l);
            }
        }
        InsertUserMatches(fallbackLdms, features, matches, umats);
    }

    // Leave only matches we are sure are from the same panorama
    auto filtered = cvd::leaveBiggestComponent(features, matches, confThresh);

    // Remove the images that aren't going to get matched
    umats = KeepByIndex(umats, filtered);
    if (not masks.empty()) {
        umats = KeepByIndex(masks, filtered);
    }

    //////////////////////////////////
    ///// estimate camera params /////
    //////////////////////////////////
    float warpedScale{1.F};
    auto cams = EstimateCameras(features, matches, confThresh, warpedScale);

    //////////////////////
    //// compose pano ////
    //////////////////////
    result_ = ComposePano(umats, cams, seamWorkAspect, seamScale, warpedScale);

    return result_;
}

/////// HELPER FUNCTIONS ///////
static ImgList ScaleImages(
    const ImgList& imgs, float scale, cv::InterpolationFlags interp)
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
    float workScale,
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

    scaledImgs.clear();
    scaledMasks.clear();

    return features;
}

static void MatchFeatures(float confThresh, FeatureList& fl, MatchesList& ml)
{
    // TODO: Full affine? BestOf2 (non-affine)? Hamming (from features2d)?
    auto matcher =
        cv::makePtr<cvd::AffineBestOf2NearestMatcher>(false, false, confThresh);
    (*matcher)(fl, ml);
    matcher->collectGarbage();
}

static int FindFeatureIndex(int idx, const FeatureList& fl)
{
    int it{0};
    for (const auto& f : fl) {
        if (idx == f.img_idx) {
            return it;
        }
        it++;
    }
    return -1;
}

static int FindMatchesIndex(int srcIdx, int dstIdx, const MatchesList& ml)
{
    int it{0};
    for (const auto& m : ml) {
        if (m.src_img_idx == srcIdx and m.dst_img_idx == dstIdx) {
            return it;
        }
        it++;
    }
    return -1;
}

static int InsertLandmarks(
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
        fl.emplace_back(f);
    }

    // Feature already exists, so append
    else {
        size = fl[idx].keypoints.size();
        for (const auto& ldm : lpLdms) {
            fl[idx].keypoints.emplace_back(ldm[0], ldm[1], 31);
        }
    }

    return size;
}

static void InsertUserMatches(
    const LdmPairList& lpl,
    FeatureList& fl,
    MatchesList& ml,
    const ImgList& imgs)
{
    // For each LandmarkPair
    for (const auto& lp : lpl) {
        // Insert landmarks into feature list
        auto srcSize = InsertLandmarks(lp.srcIdx, lp.srcLdms, fl, imgs);
        auto dstSize = InsertLandmarks(lp.dstIdx, lp.dstLdms, fl, imgs);

        // Make a new MatchesInfo if one doesnt exist
        // src->dest
        auto idxS2D = FindMatchesIndex(lp.srcIdx, lp.dstIdx, ml);
        if (idxS2D < 0) {
            idxS2D = ml.size();
            cvd::MatchesInfo match;
            match.src_img_idx = lp.srcIdx;
            match.dst_img_idx = lp.dstIdx;
            ml.push_back(match);
        }
        // dest->src
        auto idxD2S = FindMatchesIndex(lp.dstIdx, lp.srcIdx, ml);
        if (idxD2S < 0) {
            idxD2S = ml.size();
            cvd::MatchesInfo match;
            match.src_img_idx = lp.dstIdx;
            match.dst_img_idx = lp.srcIdx;
            ml.push_back(match);
        }

        // Get the matches
        auto matchS2D = ml[idxS2D];
        auto matchD2S = ml[idxD2S];

        // Add the landmark indices to the matches info
        for (int i = 0; size_t(i) < lp.srcLdms.size(); i++) {
            matchS2D.matches.emplace_back(srcSize + i, dstSize + i, 0);
            matchD2S.matches.emplace_back(dstSize + i, srcSize + i, 0);
        }

        // Compute homography
        matchS2D = ComputeHomography(matchS2D, fl);

        // Copy MatchInfo properties
        matchD2S.inliers_mask = matchS2D.inliers_mask;
        matchD2S.confidence = matchS2D.confidence;
        matchD2S.num_inliers = matchS2D.num_inliers;
        matchD2S.H = matchS2D.H.inv();

        // Reassign to the matches list
        ml[idxS2D] = matchS2D;
        ml[idxD2S] = matchD2S;
    }
}

template <typename ObjList, typename IndexList>
ObjList KeepByIndex(const ObjList& ol, const IndexList& il)
{
    // Nothing to remove
    if (ol.empty()) {
        return ol;
    }

    // Move to new list
    ObjList ret;
    for (const auto& i : il) {
        ret.emplace_back(ol[i]);
    }
    return ret;
}

static CameraList EstimateCameras(
    const FeatureList& fl,
    const MatchesList& ml,
    float confThresh,
    float& warpedScale)
{
    // estimate homography in global frame
    auto bundleAdjuster = cv::makePtr<cvd::BundleAdjusterAffinePartial>();
    auto estimator = cv::makePtr<cvd::AffineBasedEstimator>();
    CameraList cameras;
    if (!(*estimator)(fl, ml, cameras)) {
        throw std::runtime_error("Could not estimate camera homography");
    }

    // Convert rotation matrix to floats
    for (auto& c : cameras) {
        cv::Mat R;
        c.R.convertTo(R, CV_32F);
        c.R = R;
    }

    // Perform bundle adjustment
    bundleAdjuster->setConfThresh(confThresh);
    if (!(*bundleAdjuster)(fl, ml, cameras)) {
        throw std::runtime_error("Failed bundle adjustment");
    }

    // Find median focal length and use it as final image scale
    std::vector<float> focals;
    for (const auto& c : cameras) {
        focals.push_back(static_cast<float>(c.focal));
    }
    std::nth_element(
        focals.begin(), focals.begin() + focals.size() / 2, focals.end());
    warpedScale = focals[focals.size() / 2];

    // Wave correct rotations
    auto doWaveCorrect{false};
    auto waveCorrectKind{cvd::WaveCorrectKind::WAVE_CORRECT_HORIZ};
    if (doWaveCorrect) {
        std::vector<cv::Mat> rmats;
        for (const auto& c : cameras) {
            rmats.push_back(c.R.clone());
        }
        cvd::waveCorrect(rmats, waveCorrectKind);
        for (size_t i = 0; i < cameras.size(); ++i) {
            cameras[i].R = rmats[i];
        }
    }
    return cameras;
}

static cv::Mat ComposePano(
    const ImgList& imgs,
    const CameraList& cams,
    float seamAspect,
    float seamScale,
    float warpScale)
{
    // Prepare seam images
    auto seamImgs = ScaleImages(imgs, seamScale);

    // Prepare seam seamMasks
    ImgList seamMasks;
    seamMasks.reserve(seamImgs.size());
    for (const auto& i : seamImgs) {
        auto m = cv::UMat(i.size(), CV_8U);
        m.setTo(cv::Scalar::all(255));
        seamMasks.push_back(m);
    }

    // Setup warper
    auto warperCreator = cv::makePtr<cv::AffineWarper>();
    auto warper = warperCreator->create(warpScale * seamAspect);
    cv::InterpolationFlags interpFlags{cv::INTER_LINEAR};

    // Warp images and their seam masks
    ImgList imgsWarped(imgs.size());
    ImgList masksWarped(imgs.size());
    std::vector<cv::Point> corners(imgs.size());
    std::vector<cv::Size> sizes(imgs.size());
    for (size_t i = 0; i < seamImgs.size(); ++i) {
        cv::Mat_<float> K;
        cams[i].K().convertTo(K, CV_32F);
        K(0, 0) *= seamAspect;
        K(0, 2) *= seamAspect;
        K(1, 1) *= seamAspect;
        K(1, 2) *= seamAspect;

        corners[i] = warper->warp(
            seamImgs[i], K, cams[i].R, interpFlags, cv::BORDER_REFLECT,
            imgsWarped[i]);
        sizes[i] = imgsWarped[i].size();

        warper->warp(
            seamMasks[i], K, cams[i].R, cv::INTER_NEAREST, cv::BORDER_CONSTANT,
            masksWarped[i]);
    }

    // Compensate exposure before finding seams
    // TODO: Make into a user parameter
    cv::Ptr<cvd::ExposureCompensator> exposureComp =
        cv::makePtr<cvd::NoExposureCompensator>();
    exposureComp->feed(corners, imgsWarped, masksWarped);
    for (size_t i = 0; i < imgsWarped.size(); ++i) {
        exposureComp->apply(int(i), corners[i], imgsWarped[i], masksWarped[i]);
    }

    // Find seams
    cv::Ptr<cvd::SeamFinder> seamFinder = cv::makePtr<cvd::GraphCutSeamFinder>(
        cvd::GraphCutSeamFinderBase::COST_COLOR);
    ImgList imgsWarpedF(imgsWarped.size());
    for (size_t i = 0; i < imgsWarped.size(); ++i) {
        imgsWarped[i].convertTo(imgsWarpedF[i], CV_32F);
    }
    seamFinder->find(imgsWarpedF, corners, masksWarped);

    // Release unused memory
    seamImgs.clear();
    imgsWarped.clear();
    imgsWarpedF.clear();
    seamMasks.clear();

    ///// Compositing /////
    // Setup compositing scale
    float composeScale{1};
    float composeResol{cv::Stitcher::ORIG_RESOL};
    if (composeResol > 0) {
        auto area = static_cast<float>(imgs[0].size().area());
        composeScale = std::min(1.F, std::sqrt(composeResol * 1e6F / area));
    }

    // Compute relative scales
    auto composeWorkAspect = composeScale / seamScale;

    // Update warper scale factor
    warper = warperCreator->create(warpScale * composeWorkAspect);

    // Update corners and sizes
    CameraList camsScaled(cams);
    for (size_t i = 0; i < imgs.size(); ++i) {
        // Update intrinsics
        camsScaled[i].ppx *= composeWorkAspect;
        camsScaled[i].ppy *= composeWorkAspect;
        camsScaled[i].focal *= composeWorkAspect;

        // Update corner and size
        auto sz = imgs[i].size();
        if (std::abs(composeScale - 1) > 1e-1F) {
            sz.width = cvRound(static_cast<float>(sz.width) * composeScale);
            sz.height = cvRound(static_cast<float>(sz.height) * composeScale);
        }

        cv::UMat K;
        camsScaled[i].K().convertTo(K, CV_32F);
        auto roi = warper->warpRoi(sz, K, camsScaled[i].R);
        corners[i] = roi.tl();
        sizes[i] = roi.size();
    }

    // Setup blender
    cv::Ptr<cvd::Blender> blender = cv::makePtr<cvd::MultiBandBlender>(false);
    blender->prepare(corners, sizes);

    // Composite all images
    cv::UMat img;
    cv::UMat imgWarped;
    cv::UMat imgWarpedS;
    cv::UMat dilatedMask;
    cv::UMat seamMask;
    cv::UMat mask;
    cv::UMat maskWarped;
    for (size_t idx = 0; idx < imgs.size(); ++idx) {
        // Scale image to compose scale
        if (std::abs(composeScale - 1) > 1e-1F) {
            cv::resize(
                imgs[idx], img, cv::Size(), composeScale, composeScale,
                cv::INTER_LINEAR);
        } else {
            img = imgs[idx];
        }
        auto imgSize = img.size();

        // Convert K to float
        cv::UMat K;
        camsScaled[idx].K().convertTo(K, CV_32F);

        // Warp the current image
        warper->warp(
            img, K, cams[idx].R, interpFlags, cv::BORDER_REFLECT, imgWarped);

        // Warp the current image mask
        mask.create(imgSize, CV_8U);
        mask.setTo(cv::Scalar::all(255));
        warper->warp(
            mask, K, cams[idx].R, cv::INTER_NEAREST, cv::BORDER_CONSTANT,
            maskWarped);

        // Compensate exposure
        exposureComp->apply(idx, corners[idx], imgWarped, maskWarped);

        imgWarped.convertTo(imgWarpedS, CV_16S);
        imgWarped.release();
        img.release();
        mask.release();

        // Make sure seam mask has proper size
        cv::dilate(masksWarped[idx], dilatedMask, cv::UMat());
        cv::resize(
            dilatedMask, seamMask, maskWarped.size(), 0, 0, cv::INTER_LINEAR);
        cv::bitwise_and(seamMask, maskWarped, maskWarped);

        // Blend the current image
        blender->feed(imgWarpedS, maskWarped, corners[idx]);
    }

    cv::UMat result;
    cv::UMat resultMask;
    blender->blend(result, resultMask);

    // Preliminary result is in CV_16SC3 format, but all values are in [0,255]
    // range, so convert it to avoid user confusing
    cv::Mat pano;
    result.convertTo(pano, CV_8U);
    return pano;
}

static cvd::MatchesInfo ComputeHomography(
    cvd::MatchesInfo matchesInfo, const FeatureList& fl)
{
    // Construct point-point correspondences for transform estimation
    cv::Mat srcPoints(
        1, static_cast<int>(matchesInfo.matches.size()), CV_32FC2);
    cv::Mat dstPoints(
        1, static_cast<int>(matchesInfo.matches.size()), CV_32FC2);
    for (size_t i = 0; i < matchesInfo.matches.size(); ++i) {
        const cv::DMatch& m = matchesInfo.matches[i];
        srcPoints.at<cv::Point2f>(0, static_cast<int>(i)) =
            fl[matchesInfo.src_img_idx].keypoints[m.queryIdx].pt;
        dstPoints.at<cv::Point2f>(0, static_cast<int>(i)) =
            fl[matchesInfo.dst_img_idx].keypoints[m.trainIdx].pt;
    }
    matchesInfo.H = cv::estimateAffinePartial2D(
        srcPoints, dstPoints, matchesInfo.inliers_mask);

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