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

#include <iostream>

#include <opencv2/imgproc.hpp>
#include <opencv2/stitching.hpp>
#include <opencv2/stitching/detail/matchers.hpp>
#include "opencv2/stitching/warpers.hpp"

#include "opencv2/core/ocl.hpp"

using namespace rt;

// Make new function that returns void call it ComputerImagesFeatures
// It was take the same types as that function takes
//



void ImageStitcher::setLandmarks(std::vector<std::pair<float, float> > &features1,
                                 std::vector<std::pair<float, float> > &features2) {
    features1_ = features1;
    features2_ = features2;
}

std::vector<cv::detail::ImageFeatures> ImageStitcher::findFeatures(double& work_scale_, double seam_work_aspect_, std::vector<cv::UMat>& seam_est_imgs_, std::vector<cv::Size>& full_img_sizes_) {
    double registr_resol_{0.6};
    double seam_est_resol_{0.1};
    double seam_scale_ = 1;
    bool is_work_scale_set = false;
    bool is_seam_scale_set = false;
    cv::Ptr<cv::detail::FeaturesFinder> features_finder_ = new cv::detail::OrbFeaturesFinder();
    std::vector<cv::UMat> feature_find_imgs(imgs_.size());
    std::vector<cv::UMat> feature_find_masks(masks_.size());

    std::vector<cv::detail::ImageFeatures> features_(imgs_.size());

    for (size_t i = 0; i < imgs_.size(); ++i) {
        full_img_sizes_[i] = imgs_[i].size();
        if (registr_resol_ < 0) {
            feature_find_imgs[i] = imgs_[i];
            work_scale_ = 1;
            is_work_scale_set = true;
        } else {
            if (!is_work_scale_set) {
                work_scale_ = std::min(
                        1.0, std::sqrt(
                                registr_resol_ * 1e6 / full_img_sizes_[i].area()));
                is_work_scale_set = true;
            }
            cv::resize(
                    imgs_[i], feature_find_imgs[i], cv::Size(), work_scale_,
                    work_scale_, cv::INTER_LINEAR);
        }
        if (!is_seam_scale_set) {
            seam_scale_ = std::min(
                    1.0,
                    std::sqrt(seam_est_resol_ * 1e6 / full_img_sizes_[i].area()));
            seam_work_aspect_ = seam_scale_ / work_scale_;
            is_seam_scale_set = true;
        }

        if (!masks_.empty()) {
            cv::resize(
                    masks_[i], feature_find_masks[i], cv::Size(), work_scale_,
                    work_scale_, cv::INTER_NEAREST);
        }
        features_[i].img_idx = (int)i;
        // LOGLN("Features in image #" << i+1 << ": " <<
        // features_[i].keypoints.size());

        cv::resize(
                imgs_[i], seam_est_imgs_[i], cv::Size(), seam_scale_, seam_scale_,
                cv::INTER_LINEAR);
    }

    // find features possibly in parallel
    // if (rois_.empty())
    (*features_finder_)(feature_find_imgs, features_);
    //cv::detail::computeImageFeatures(features_finder_, feature_find_imgs, features_, feature_find_masks);

    // else
    //(*features_finder_)(feature_find_imgs, features_, feature_find_rois);

    // Do it to save memory
    feature_find_imgs.clear();
    feature_find_masks.clear();

    return features_;
}

std::vector<cv::detail::MatchesInfo> ImageStitcher::findMatches(double conf_thresh_, std::vector<cv::UMat>& seam_est_imgs_, std::vector<cv::Size>& full_img_sizes_, std::vector<cv::detail::ImageFeatures>& features_) {
    std::vector<cv::detail::MatchesInfo> pairwise_matches_;
    cv::Ptr<cv::detail::FeaturesMatcher> features_matcher_ = cv::makePtr<cv::detail::AffineBestOf2NearestMatcher>(false, false);
    cv::UMat matching_mask_;
    // LOGLN("Finding features, time: " << ((getTickCount() - t) /
    // getTickFrequency()) << " sec");

    (*features_matcher_)(features_, pairwise_matches_, matching_mask_);
    features_matcher_->collectGarbage();
    //(stitcher->featuresMatcher())->collectGarbage();
    // LOGLN("Pairwise matching, time: " << ((getTickCount() - t) /
    // getTickFrequency()) << " sec");

    // Leave only images we are sure are from the same panorama
    auto indices_ = cv::detail::leaveBiggestComponent(
            features_, pairwise_matches_, (float)conf_thresh_);
    std::vector<cv::UMat> seam_est_imgs_subset;
    std::vector<cv::UMat> imgs_subset;
    std::vector<cv::Size> full_img_sizes_subset;
    for (size_t i = 0; i < indices_.size(); ++i) {
        imgs_subset.push_back(imgs_[indices_[i]]);
        seam_est_imgs_subset.push_back(seam_est_imgs_[indices_[i]]);
        full_img_sizes_subset.push_back(full_img_sizes_[indices_[i]]);
    }
    seam_est_imgs_ = seam_est_imgs_subset;
    imgs_ = imgs_subset;
    full_img_sizes_ = full_img_sizes_subset;

    return pairwise_matches_;
}

std::vector<cv::detail::CameraParams> ImageStitcher::estimateCameraParams(double conf_thresh_, float& warped_image_scale_, std::vector<cv::detail::ImageFeatures>& features_,
                                                                        std::vector<cv::detail::MatchesInfo>& pairwise_matches_) {
    //////////////////////////////////
    ///// Estimate camera params /////
    //////////////////////////////////
    // estimate homography in global frame
    cv::Ptr<cv::detail::BundleAdjusterBase> bundle_adjuster_ = cv::makePtr<cv::detail::BundleAdjusterAffinePartial>();
    cv::Ptr<cv::detail::Estimator> estimator_ =
            cv::makePtr<cv::detail::AffineBasedEstimator>();
    std::vector<cv::detail::CameraParams> cameras_;
    if (!(*estimator_)(features_, pairwise_matches_, cameras_)) {
        throw std::runtime_error("Could not estimate camera homography");
    }

    for (size_t i = 0; i < cameras_.size(); ++i) {
        cv::Mat R;
        cameras_[i].R.convertTo(R, CV_32F);
        cameras_[i].R = R;
        // LOGLN("Initial intrinsic parameters #" << indices_[i] + 1 << ":\n "
        // << cameras_[i].K());
    }

    bundle_adjuster_->setConfThresh(conf_thresh_);
    if (!(*bundle_adjuster_)(features_, pairwise_matches_, cameras_)) {
        throw std::runtime_error("Failed bundle adjustment");
    }

    // Find median focal length and use it as final image scale
    std::vector<double> focals;
    for (size_t i = 0; i < cameras_.size(); ++i) {
        // LOGLN("Camera #" << indices_[i] + 1 << ":\n" << cameras_[i].K());
        focals.push_back(cameras_[i].focal);
    }

    std::sort(focals.begin(), focals.end());
    bool do_wave_correct_{false};
    cv::detail::WaveCorrectKind wave_correct_kind_{
            cv::detail::WaveCorrectKind::WAVE_CORRECT_HORIZ};
    if (focals.size() % 2 == 1)
        warped_image_scale_ = static_cast<float>(focals[focals.size() / 2]);
    else
        warped_image_scale_ =
                static_cast<float>(
                        focals[focals.size() / 2 - 1] + focals[focals.size() / 2]) *
                0.5f;

    if (do_wave_correct_) {
        std::vector<cv::Mat> rmats;
        for (size_t i = 0; i < cameras_.size(); ++i)
            rmats.push_back(cameras_[i].R.clone());
        cv::detail::waveCorrect(rmats, wave_correct_kind_);
        for (size_t i = 0; i < cameras_.size(); ++i)
            cameras_[i].R = rmats[i];
    }
    return cameras_;
}

cv::Mat ImageStitcher::composePano(double seam_work_aspect_, float warped_image_scale_, double work_scale_, std::vector<cv::UMat>& seam_est_imgs_, std::vector<cv::detail::CameraParams>& cameras_, std::vector<cv::Size>& full_img_sizes_) {
    //////////////////////
    //// compose pano ////
    //////////////////////
    cv::Mat pano;

    double compose_resol_{cv::Stitcher::ORIG_RESOL};
    std::vector<cv::Point> corners(imgs_.size());
    std::vector<cv::UMat> masks_warped(imgs_.size());
    std::vector<cv::UMat> images_warped(imgs_.size());
    std::vector<cv::Size> sizes(imgs_.size());
    std::vector<cv::UMat> masks(imgs_.size());

    // Prepare image masks
    for (size_t i = 0; i < imgs_.size(); ++i) {
        masks[i].create(seam_est_imgs_[i].size(), CV_8U);
        masks[i].setTo(cv::Scalar::all(255));
    }

    // Warp images and their masks
    cv::Ptr<cv::WarperCreator> warper_ = cv::makePtr<cv::AffineWarper>();
    cv::Ptr<cv::detail::RotationWarper> w =
            (warper_)->create(float(warped_image_scale_ * seam_work_aspect_));
    cv::InterpolationFlags interp_flags_{cv::INTER_LINEAR};
    for (size_t i = 0; i < imgs_.size(); ++i) {
        cv::Mat_<float> K;
        cameras_[i].K().convertTo(K, CV_32F);
        K(0, 0) *= (float)seam_work_aspect_;
        K(0, 2) *= (float)seam_work_aspect_;
        K(1, 1) *= (float)seam_work_aspect_;
        K(1, 2) *= (float)seam_work_aspect_;

        corners[i] = w->warp(
                seam_est_imgs_[i], K, cameras_[i].R, interp_flags_,
                cv::BORDER_REFLECT, images_warped[i]);
        sizes[i] = images_warped[i].size();

        w->warp(
                masks[i], K, cameras_[i].R, cv::INTER_NEAREST, cv::BORDER_CONSTANT,
                masks_warped[i]);
    }

    // Compensate exposure before finding seams
    cv::Ptr<cv::detail::ExposureCompensator> exposure_comp_ = cv::makePtr<cv::detail::NoExposureCompensator>();
    exposure_comp_->feed(corners, images_warped, masks_warped);
    for (size_t i = 0; i < imgs_.size(); ++i)
        exposure_comp_->apply(
                int(i), corners[i], images_warped[i], masks_warped[i]);

    // Find seams
    cv::Ptr<cv::detail::SeamFinder> seam_finder_ = cv::makePtr<cv::detail::GraphCutSeamFinder>(cv::detail::GraphCutSeamFinderBase::COST_COLOR);
    std::vector<cv::UMat> images_warped_f(imgs_.size());
    for (size_t i = 0; i < imgs_.size(); ++i)
        images_warped[i].convertTo(images_warped_f[i], CV_32F);
    seam_finder_->find(images_warped_f, corners, masks_warped);

    // Release unused memory
    seam_est_imgs_.clear();
    images_warped.clear();
    images_warped_f.clear();
    masks.clear();

    // Compositing
    cv::UMat img_warped, img_warped_s;
    cv::UMat dilated_mask, seam_mask, mask, mask_warped;

    // double compose_seam_aspect = 1;
    double compose_work_aspect = 1;
    bool is_blender_prepared = false;

    double compose_scale = 1;
    bool is_compose_scale_set = false;

    std::vector<cv::detail::CameraParams> cameras_scaled(cameras_);

    cv::UMat full_img, img;
    cv::Ptr<cv::detail::Blender> blender_ = cv::makePtr<cv::detail::MultiBandBlender>(false);
    for (size_t img_idx = 0; img_idx < imgs_.size(); ++img_idx) {
        // Read image and resize it if necessary
        full_img = imgs_[img_idx];
        if (!is_compose_scale_set) {
            if (compose_resol_ > 0)
                compose_scale = std::min(
                        1.0,
                        std::sqrt(compose_resol_ * 1e6 / full_img.size().area()));
            is_compose_scale_set = true;

            // Compute relative scales
            // compose_seam_aspect = compose_scale / seam_scale_;
            compose_work_aspect = compose_scale / work_scale_;

            // Update warped image scale
            float warp_scale =
                    static_cast<float>(warped_image_scale_ * compose_work_aspect);
            w = warper_->create(warp_scale);

            // Update corners and sizes
            for (size_t i = 0; i < imgs_.size(); ++i) {
                // Update intrinsics
                cameras_scaled[i].ppx *= compose_work_aspect;
                cameras_scaled[i].ppy *= compose_work_aspect;
                cameras_scaled[i].focal *= compose_work_aspect;

                // Update corner and size
                cv::Size sz = full_img_sizes_[i];
                if (std::abs(compose_scale - 1) > 1e-1) {
                    sz.width =
                            cvRound(full_img_sizes_[i].width * compose_scale);
                    sz.height =
                            cvRound(full_img_sizes_[i].height * compose_scale);
                }

                cv::UMat K;
                cameras_scaled[i].K().convertTo(K, CV_32F);
                cv::Rect roi = w->warpRoi(sz, K, cameras_scaled[i].R);
                corners[i] = roi.tl();
                sizes[i] = roi.size();
            }
        }
        if (std::abs(compose_scale - 1) > 1e-1) {
            resize(
                    full_img, img, cv::Size(), compose_scale, compose_scale,
                    cv::INTER_LINEAR);
        } else
            img = full_img;
        full_img.release();
        cv::Size img_size = img.size();

        cv::UMat K;
        cameras_scaled[img_idx].K().convertTo(K, CV_32F);

        // Warp the current image
        w->warp(
                img, K, cameras_[img_idx].R, interp_flags_, cv::BORDER_REFLECT,
                img_warped);

        // Warp the current image mask
        mask.create(img_size, CV_8U);
        mask.setTo(cv::Scalar::all(255));
        w->warp(
                mask, K, cameras_[img_idx].R, cv::INTER_NEAREST,
                cv::BORDER_CONSTANT, mask_warped);

        // Compensate exposure
        exposure_comp_->apply(
                (int)img_idx, corners[img_idx], img_warped, mask_warped);

        img_warped.convertTo(img_warped_s, CV_16S);
        img_warped.release();
        img.release();
        mask.release();

        // Make sure seam mask has proper size
        cv::dilate(masks_warped[img_idx], dilated_mask, cv::UMat());
        cv::resize(
                dilated_mask, seam_mask, mask_warped.size(), 0, 0,
                cv::INTER_LINEAR);

        cv::bitwise_and(seam_mask, mask_warped, mask_warped);

        // Blender
        if (!is_blender_prepared) {
            blender_->prepare(corners, sizes);
            is_blender_prepared = true;
        }

        // Blend the current image
        blender_->feed(img_warped_s, mask_warped, corners[img_idx]);
    }

    cv::UMat result;
    cv::UMat result_mask_;
    blender_->blend(result, result_mask_);

    // Preliminary result is in CV_16SC3 format, but all values are in [0,255]
    // range, so convert it to avoid user confusing
    result.convertTo(pano, CV_8U);
    return pano;
}

// The implementation of this function is a modified version of
// cv::Stitcher::stitch()
cv::Mat ImageStitcher::compute()
{
    //////////////////////
    //// match images ////
    //////////////////////
    if (imgs_.size() < 2) {
        throw std::runtime_error("Not enough images to perform stitching");
    }

    double conf_thresh_{1};
    double work_scale_ = 1;
    double seam_work_aspect_ = 1;

    std::vector<cv::UMat> seam_est_imgs_(imgs_.size());

    std::vector<cv::Size> full_img_sizes_(imgs_.size());

    auto features_ = findFeatures(work_scale_, seam_work_aspect_, seam_est_imgs_, full_img_sizes_);

    auto pairwise_matches_ = findMatches(conf_thresh_, seam_est_imgs_, full_img_sizes_, features_);

    if ((int)imgs_.size() < 2) {
        // LOGLN("Need more images");
        throw std::runtime_error(
            "Not enough matched images to perform stitching");
    }

    //////////////////////////////////
    ///// Estimate camera params /////
    //////////////////////////////////
    float warped_image_scale_;
    std::vector<cv::detail::CameraParams> cameras_ = estimateCameraParams(conf_thresh_, warped_image_scale_, features_, pairwise_matches_);

    //////////////////////
    //// compose pano ////
    //////////////////////
    return composePano(seam_work_aspect_, warped_image_scale_, work_scale_, seam_est_imgs_, cameras_, full_img_sizes_);
}