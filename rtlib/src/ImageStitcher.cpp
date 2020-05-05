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

#include <boost/filesystem.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace rt;

int search_img_index_(int img_index);
int search_(cv::detail::ImageFeatures& features, std::pair<float, float>& point);
void filter_matched_features_(cv::detail::ImageFeatures& features, std::vector<std::pair<float, float> >& points1, std::vector<std::pair<float, float> >& points2);
void reduce_img_points_(const double& work_scale, std::vector<std::pair<float, float> > &features1, std::vector<std::pair<float, float> > &features2);

void ImageStitcher::setGenerateLandmarks(bool generate){
    generateLandmarks_ = generate;
}

void ImageStitcher::setOption(int option){
    option_ = option;
}

void ImageStitcher::compute_homography_(cv::detail::MatchesInfo& matches_info){
    // Construct point-point correspondences for transform estimation
    cv::Mat src_points(1, static_cast<int>(matches_info.matches.size()), CV_32FC2);
    cv::Mat dst_points(1, static_cast<int>(matches_info.matches.size()), CV_32FC2);
    for (size_t i = 0; i < matches_info.matches.size(); ++i)
    {
        const cv::DMatch &m = matches_info.matches[i];
        src_points.at<cv::Point2f>(0, static_cast<int>(i)) = allFeatures_[matches_info.src_img_idx].keypoints[m.queryIdx].pt;
        dst_points.at<cv::Point2f>(0, static_cast<int>(i)) = allFeatures_[matches_info.dst_img_idx].keypoints[m.trainIdx].pt;
    }
    matches_info.H = estimateAffinePartial2D(src_points, dst_points, matches_info.inliers_mask);

    if (matches_info.H.empty()) {
        // could not find transformation
        matches_info.confidence = 0;
        matches_info.num_inliers = 0;
        return;
    }

    // Find number of inliers
    matches_info.num_inliers = 0;
    for (size_t i = 0; i < matches_info.inliers_mask.size(); ++i) {
        if (matches_info.inliers_mask[i]) {
            matches_info.num_inliers++;
        }
    }

    // These coeffs are from paper M. Brown and D. Lowe. "Automatic Panoramic
    // Image Stitching using Invariant Features"
    matches_info.confidence =
            matches_info.num_inliers / (8 + 0.3 * matches_info.matches.size());

    // should we remove matches between too close images?
    // matches_info.confidence = matches_info.confidence > 3. ? 0. : matches_info.confidence;

    // extend H to represent linear transformation in homogeneous coordinates
    matches_info.H.push_back(cv::Mat::zeros(1, 3, CV_64F));
    matches_info.H.at<double>(2, 2) = 1;
}

int search_(cv::detail::ImageFeatures& features, std::pair<float, float>& point){
        for(int j = 0; j < features.keypoints.size(); j++){
            if(std::abs(features.keypoints[j].pt.x - point.first) < 0.001){
                if(std::abs(features.keypoints[j].pt.y - point.second) < 0.001){
                        return j;
                }
            }
        }
        return -1;
    }

void filter_matched_features_(cv::detail::ImageFeatures& features, std::vector<std::pair<float, float> >& points1, std::vector<std::pair<float, float> >& points2){
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

void reduce_img_points_(const double& work_scale, std::vector<std::pair<float, float> > &features1, std::vector<std::pair<float, float> > &features2){
    for(int i = 0; i < features1.size(); i++){
        features1[i].first = features1[i].first * work_scale;
        features1[i].second = features1[i].second * work_scale;
        features2[i].first = features1[i].first * work_scale;
        features2[i].second = features2[i].second * work_scale;
    }
}

int ImageStitcher::search_img_index_(int img_index) {
    for(int i = 0; i < allFeatures_.size(); i++){
        if(img_index == allFeatures_[i].img_idx){
            return i;
        }
    }
    return -1;
}

void ImageStitcher::insert_user_matches_(const LandmarkPair& ldm_pair){
    // Need to have the images scaled for the work scale
    double registr_resol_{0.6};
    double work_scale_ = 1;
    cv::Ptr<cv::FeatureDetector> orb = cv::ORB::create();
    int src_size, dst_size;
    // Populate the keypoints for the src image first
    cv::detail::ImageFeatures img1_features;
    int img1_idx = search_img_index_(ldm_pair.srcIdx);
    // Check if the src image is already in the features vector
    if(img1_idx < 0){
        // Creates a new feature and puts it into all_features_
        img1_features.img_idx = ldm_pair.srcIdx;
        img1_features.img_size = cv::Size(imgs_[ldm_pair.srcIdx].size().width, imgs_[ldm_pair.srcIdx].size().height);
        for(int i = 0; i < ldm_pair.srcLdms.size(); i++){
            img1_features.keypoints.emplace_back(ldm_pair.srcLdms[i].first, ldm_pair.srcLdms[i].second, 31);
        }

        //May need to compute the descriptors

        /*orb->detectAndCompute(smaller_imgs[0], cv::UMat(), feature_img1.keypoints, feature_img1.descriptors, true);
        filterMatchedFeatures(feature_img1, features1, features2);*/
        src_size = 0;
        allFeatures_.push_back(img1_features);
    }
    else{
        src_size = allFeatures_[img1_idx].keypoints.size();
        // Appends the new landmarks
        for(int i = 0; i < ldm_pair.srcLdms.size(); i++){
            allFeatures_[img1_idx].keypoints.emplace_back(ldm_pair.srcLdms[i].first, ldm_pair.srcLdms[i].second, 31);
        }
    }

    // Populate the keypoints for the dst image
    cv::detail::ImageFeatures img2_features;
    int img2_idx = search_img_index_(ldm_pair.dstIdx);
    // Check if the src image is already in the features vector
    if(img2_idx < 0){
        // Creates a new feature and puts it into all_features_
        img2_features.img_idx = ldm_pair.dstIdx;
        img2_features.img_size = cv::Size(imgs_[ldm_pair.dstIdx].size().width, imgs_[ldm_pair.dstIdx].size().height);
        for(int i = 0; i < ldm_pair.dstLdms.size(); i++){
            img2_features.keypoints.emplace_back(ldm_pair.dstLdms[i].first, ldm_pair.dstLdms[i].second, 31);
        }

        //May need to compute the descriptors with orb->detectAndCompute with useProvidedKepoints = true

        /*orb->detectAndCompute(smaller_imgs[0], cv::UMat(), feature_img1.keypoints, feature_img1.descriptors, true);
        filterMatchedFeatures(feature_img1, features1, features2);*/
        dst_size = 0;
        allFeatures_.push_back(img2_features);
    }
    else{
        dst_size = allFeatures_[img2_idx].keypoints.size();
        // Appends the new landmarks
        for(int i = 0; i < ldm_pair.dstLdms.size(); i++){
            allFeatures_[img2_idx].keypoints.emplace_back(ldm_pair.dstLdms[i].first, ldm_pair.dstLdms[i].second, 31);
        }
        //May need to compute the descriptors
    }

    // Populate the matches and put them into pairwise matches
    int src_dst_idx = search_matches_(ldm_pair.srcIdx, ldm_pair.dstIdx);
    cv::detail::MatchesInfo matches1 = allPairwiseMatches_[src_dst_idx];
    for(int i = 0; i < ldm_pair.srcLdms.size(); i++){
        matches1.matches.emplace_back(src_size + i, dst_size + i, 0);
    }
    compute_homography_(matches1);
    allPairwiseMatches_[src_dst_idx] = matches1;

    int dst_src_idx = search_matches_(ldm_pair.dstIdx, ldm_pair.srcIdx);
    cv::detail::MatchesInfo matches2 = allPairwiseMatches_[dst_src_idx];
    for(int i = 0; i < matches1.matches.size(); i++){
        matches2.matches.emplace_back(matches1.matches[i].trainIdx, matches1.matches[i].queryIdx, 0);
    }
    matches2.inliers_mask = matches1.inliers_mask;
    matches2.confidence = matches1.confidence;
    matches2.num_inliers = matches1.num_inliers;
    matches2.H = matches1.H.inv();
    allPairwiseMatches_[dst_src_idx] = matches2;
}

// This will probably need to be changed to account for having more than 2 images
void ImageStitcher::setLandmarks(std::vector<std::string> ldmFiles){

    if(ldmFiles.size() >= imgs_.size()){
        throw std::runtime_error(
                "Too many landmark files.");
    }
    for(int i = 0; i < ldmFiles.size(); i++){
        LandmarkPair ldm;
        ldm.srcIdx = i;
        ldm.dstIdx = i+1;
        std::ifstream landmarkFile;
        // Open the landmarks file
        landmarkFile.open(ldmFiles[i]);
        // Check that the landmarks file opened
        if (!landmarkFile.is_open()) {
            throw std::runtime_error(
                    "Could not open landmarks file " + ldmFiles[i] + ".");
        }

        std::pair<float, float> point1;
        std::pair<float, float> point2;
        // Read in the landmarks and store them
        landmarkFile >> point1.first;
        while (!landmarkFile.eof()) {
            landmarkFile >> point1.second;
            ldm.srcLdms.push_back(point1);
            landmarkFile >> point2.first;
            landmarkFile >> point2.second;
            ldm.dstLdms.push_back(point2);
            landmarkFile >> point1.first;
        }
        // Close the landmarks file
        landmarkFile.close();
        landmarks_.push_back(ldm);
    }
}

std::vector<cv::detail::ImageFeatures> ImageStitcher::find_features_(double& work_scale_) {
    cv::Ptr<cv::detail::FeaturesFinder> features_finder_ = new cv::detail::OrbFeaturesFinder();
    std::vector<cv::UMat> feature_find_imgs(imgs_.size());
    std::vector<cv::UMat> feature_find_masks(masks_.size());

    std::vector<cv::detail::ImageFeatures> features_(imgs_.size());

    for (size_t i = 0; i < imgs_.size(); ++i) {
        cv::resize(
                imgs_[i], feature_find_imgs[i], cv::Size(), work_scale_,
                work_scale_, cv::INTER_LINEAR);


        if (!masks_.empty()) {
            cv::resize(
                    masks_[i], feature_find_masks[i], cv::Size(), work_scale_,
                    work_scale_, cv::INTER_NEAREST);
        }
        features_[i].img_idx = static_cast<int>(i);
        // LOGLN("Features in image #" << i+1 << ": " <<
        // features_[i].keypoints.size());
    }

    // find features possibly in parallel
    // if (rois_.empty())

    (*features_finder_)(feature_find_imgs, features_);

    // else
    //(*features_finder_)(feature_find_imgs, features_, feature_find_rois);

    // Do it to save memory
    feature_find_imgs.clear();
    feature_find_masks.clear();

    return features_;
}

void ImageStitcher::find_matches_(double conf_thresh_, std::vector<cv::UMat>& seam_est_imgs_, std::vector<cv::Size>& full_img_sizes_) {
    std::vector<cv::detail::MatchesInfo> pairwise_matches_;
    auto features_matcher_ = cv::makePtr<cv::detail::AffineBestOf2NearestMatcher>(false, false);
    cv::UMat matching_mask_;
    // LOGLN("Finding features, time: " << ((getTickCount() - t) /
    // getTickFrequency()) << " sec");

    (*features_matcher_)(allFeatures_, allPairwiseMatches_, matching_mask_);
    features_matcher_->collectGarbage();
    // LOGLN("Pairwise matching, time: " << ((getTickCount() - t) /
    // getTickFrequency()) << " sec");

    // This is for option 1 with placing the matches before filtering the images
    if (option_ == 1 && !landmarks_.empty()){
        for(int i = 0; i < landmarks_.size(); i++){
            insert_user_matches_(landmarks_[i]);
        }
    }
    // Leave only images we are sure are from the same panorama
    auto indices_ = cv::detail::leaveBiggestComponent(
            allFeatures_, allPairwiseMatches_, (float)conf_thresh_);
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

}

std::vector<cv::detail::CameraParams> ImageStitcher::estimate_camera_params_(double conf_thresh_, float& warped_image_scale_, std::vector<cv::detail::ImageFeatures>& features_,
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
    if (focals.size() % 2 == 1) {
        warped_image_scale_ = static_cast<float>(focals[focals.size() / 2]);
    }
    else {
        warped_image_scale_ =
                static_cast<float>(
                        focals[focals.size() / 2 - 1] + focals[focals.size() / 2]) *
                0.5f;
    }

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

cv::Mat ImageStitcher::compose_pano_(double seam_work_aspect_, float warped_image_scale_, double work_scale_, std::vector<cv::UMat>& seam_est_imgs_, std::vector<cv::detail::CameraParams>& cameras_, std::vector<cv::Size>& full_img_sizes_) {
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
    for (size_t i = 0; i < imgs_.size(); ++i) {
        exposure_comp_->apply(
                int(i), corners[i], images_warped[i], masks_warped[i]);
    }

    // Find seams
    cv::Ptr<cv::detail::SeamFinder> seam_finder_ = cv::makePtr<cv::detail::GraphCutSeamFinder>(cv::detail::GraphCutSeamFinderBase::COST_COLOR);
    std::vector<cv::UMat> images_warped_f(imgs_.size());
    for (size_t i = 0; i < imgs_.size(); ++i) {
        images_warped[i].convertTo(images_warped_f[i], CV_32F);
    }
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
            if (compose_resol_ > 0) {
                compose_scale = std::min(
                        1.0,
                        std::sqrt(compose_resol_ * 1e6 / full_img.size().area()));
            }
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
        } else {
            img = full_img;
        }
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
            cv::detail::MatchesInfo match;
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
void ImageStitcher::printFeatures(std::string file_path){
    cv::UMat output;
    for(int i = 0; i < allFeatures_.size(); i++) {
        cv::drawKeypoints(imgs_[allFeatures_[i].img_idx], allFeatures_[i].keypoints, output, cv::Scalar(0, 255, 0),
                          cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        cv::imwrite(file_path + "features_" + std::to_string(i) + ".jpg", output);
    }
}

// Need to give a flag that will print the images within a certain folder
void ImageStitcher::printMatches(std::string file_path){
    cv::UMat matches_output;
    for(int i = 0; i < allPairwiseMatches_.size(); i++) {
        if(allPairwiseMatches_[i].src_img_idx != -1 && allPairwiseMatches_[i].dst_img_idx != -1 && allPairwiseMatches_[i].src_img_idx != allPairwiseMatches_[i].dst_img_idx) {
            if (!allPairwiseMatches_[i].matches.empty()) {
                int src = allPairwiseMatches_[i].src_img_idx;
                int dest = allPairwiseMatches_[i].dst_img_idx;
                cv::drawMatches(imgs_[src], allFeatures_[src].keypoints, imgs_[dest], allFeatures_[dest].keypoints,
                                allPairwiseMatches_[i].matches, matches_output, cv::Scalar(0, 255, 0),
                                cv::Scalar(0, 255, 0));
                cv::imwrite(file_path + "matches_" + std::to_string(i) + ".jpg",
                            matches_output);
            }
        }
    }
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

    // Currently conf_thres = 1 if using automatic stitching
    // and conf_thres = 0.1 if using user landmarks
    //double conf_thresh_{1};
    double conf_thresh_{0.1};
    double work_scale_ = 1;
    double seam_work_aspect_ = 1;

    std::vector<cv::UMat> seam_est_imgs_(imgs_.size());

    std::vector<cv::Size> full_img_sizes_(imgs_.size());

    double registr_resol_{-0.6};
    double seam_est_resol_{0.1};
    double seam_scale_ = 1;
    bool is_work_scale_set = false;
    bool is_seam_scale_set = false;

    for (size_t i = 0; i < imgs_.size(); ++i) {
        full_img_sizes_[i] = imgs_[i].size();
        if (registr_resol_ < 0) {
            work_scale_ = 1;
            is_work_scale_set = true;
        } else {
            if (!is_work_scale_set) {
                work_scale_ = std::min(
                        1.0, std::sqrt(
                                registr_resol_ * 1e6 / full_img_sizes_[i].area()));
                is_work_scale_set = true;
            }
        }
        if (!is_seam_scale_set) {
            seam_scale_ = std::min(
                    1.0,
                    std::sqrt(seam_est_resol_ * 1e6 / full_img_sizes_[i].area()));
            seam_work_aspect_ = seam_scale_ / work_scale_;
            is_seam_scale_set = true;
        }
        // LOGLN("Features in image #" << i+1 << ": " <<
        // features_[i].keypoints.size());

        cv::resize(
                imgs_[i], seam_est_imgs_[i], cv::Size(), seam_scale_, seam_scale_,
                cv::INTER_LINEAR);
    }

    // Need to add the functionality for adding the user generate landmarks/features
    // Add these landmarks/features and matches to the all_features and all_pairwise_matches vectors

    if(generateLandmarks_) {
        allFeatures_ = find_features_(work_scale_);

        find_matches_(conf_thresh_, seam_est_imgs_, full_img_sizes_);
    }
    else{
        // Creates all the possible matches
        create_matches_();
        // Only possible option for placing user landmarks is option 2
        // Change option to 2
        option_ = 2;
    }
    // This is option 2 with placing the user matches after getting the automatic matches
    if (option_ == 2 && !landmarks_.empty()){
        for(int i = 0; i < landmarks_.size(); i++){
            insert_user_matches_(landmarks_[i]);
        }
    }

    if ((int)imgs_.size() < 2) {
        // LOGLN("Need more images");
        throw std::runtime_error(
            "Not enough matched images to perform stitching");
    }

    //////////////////////////////////
    ///// Estimate camera params /////
    //////////////////////////////////
    float warped_image_scale_;
    std::vector<cv::detail::CameraParams> cameras_ = estimate_camera_params_(conf_thresh_, warped_image_scale_, allFeatures_, allPairwiseMatches_);

    //////////////////////
    //// compose pano ////
    //////////////////////
    return compose_pano_(seam_work_aspect_, warped_image_scale_, work_scale_, seam_est_imgs_, cameras_, full_img_sizes_);
}