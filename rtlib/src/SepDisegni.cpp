//
// Created by Sammy Stampley on 11/12/19.
//
//
// Created by Sammy Stampley on 10/3/19.
//

#include "SepDisegni.h"

//Constructor for the SepDisegni Class
SepDisegni::SepDisegni(){
    Mat image_;
    Mat watershededImage_;
    vector<Mat> fragmentedImage_;
}



//This takes an Input Image and sets it into your private image member
const void SepDisegni::SetImage(Mat input) {
    image_ = input;
}

//This retrieves the array of fragmented images that are stored
const vector<Mat> SepDisegni::GetFragmentedImage() {
    return fragmentedImage_;
}

vector<Mat> SepDisegni::Compute(){
    watershededImage_ = watershed_image();
    fragmentedImage_ = split_merge(watershededImage_);
    return fragmentedImage_;
}

Mat SepDisegni::watershed_image() {


    Mat src = image_;

    // Show source image
    namedWindow("Source Image", 0);
    imshow("Source Image", src);
    waitKey(0);


    // Change the background from white to black, since that will help later to extract
    // better results during the use of Distance Transform
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++) {
            if (src.at<Vec3b>(i, j) == Vec3b(255, 255, 255)) {
                src.at<Vec3b>(i, j)[0] = 0;
                src.at<Vec3b>(i, j)[1] = 0;
                src.at<Vec3b>(i, j)[2] = 0;
            }
        }
    }
    // Show output image
    namedWindow("Black Background Image", 0);
    imshow("Black Background Image", src);
    waitKey(0);


    // Create a kernel that we will use to sharpen our image
    Mat kernel = (Mat_<float>(3, 3) <<
                                    1, 1, 1,
            1, -8, 1,
            1, 1, 1); // an approximation of second derivative, a quite strong kernel

    // do the laplacian filtering as it is
    // well, we need to convert everything in something more deeper then CV_8U
    // because the kernel has some negative values,
    // and we can expect in general to have a Laplacian image with negative values
    // BUT a 8bits unsigned int (the one we are working with) can contain values from 0 to 255
    // so the possible negative number will be truncated
    Mat imgLaplacian;
    filter2D(src, imgLaplacian, CV_32F, kernel);
    Mat sharp;
    src.convertTo(sharp, CV_32F);
    Mat imgResult = sharp - imgLaplacian;
    // convert back to 8bits gray scale
    imgResult.convertTo(imgResult, CV_8UC3);
    imgLaplacian.convertTo(imgLaplacian, CV_8UC3);
    // imshow( "Laplace Filtered Image", imgLaplacian );
    namedWindow("New Sharped Image", 0);
    imshow( "New Sharped Image", imgResult );
    waitKey(0);


    //Image Median Blur

    //Median Blur Image
    medianBlur(imgResult, imgResult, 7);
    namedWindow("Blurred Image", 0);
    imshow("Blurred Image", imgResult);
    waitKey(0);


    // Create binary image from source image
    Mat bw;
    cvtColor(imgResult, bw, COLOR_BGR2GRAY);
    threshold(bw, bw, 40, 255, THRESH_BINARY | THRESH_OTSU);
    namedWindow("Binary Image", 0);
    imshow("Binary Image", bw);
    waitKey(0);





    // Perform the distance transform algorithm
    Mat dist;
    distanceTransform(bw, dist, DIST_L2, 3);
    // Normalize the distance image for range = {0.0, 1.0}
    // so we can visualize and threshold it
    normalize(dist, dist, 0, 1.0, NORM_MINMAX);
    namedWindow("Distance Transform Image", 0);
    imshow("Distance Transform Image", dist);
    waitKey(0);



    // Threshold to obtain the peaks
    // This will be the markers for the foreground objects
    threshold(dist, dist, 0.4, 1.0, THRESH_BINARY);
    // Dilate a bit the dist image
    Mat kernel1 = Mat::ones(3, 3, CV_8U);
    dilate(dist, dist, kernel1);
    namedWindow("Peaks", 0);
    imshow("Peaks", dist);
    waitKey(0);



    // Create the CV_8U version of the distance image
    // It is needed for findContours()
    Mat dist_8u;
    dist.convertTo(dist_8u, CV_8U);
    // Find total markers
    vector<vector<Point> > contours;
    findContours(dist_8u, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    // Create the marker image for the watershed algorithm
    Mat markers = Mat::zeros(dist.size(), CV_32S);
    // Draw the foreground markers
    for (size_t i = 0; i < contours.size(); i++) {
        drawContours(markers, contours, static_cast<int>(i), Scalar(static_cast<int>(i) + 1), -1);
    }
    // Draw the background marker
    circle(markers, Point(5, 5), 3, Scalar(255), -1);
    markers.convertTo(markers, CV_8U);
    namedWindow("Markers", 0);
    imshow("Markers", markers * 10000);//FIX ITTT convert to correct C_16F
    waitKey(0);
    markers.convertTo(markers, CV_32S);



    // Perform the watershed algorithm
    watershed(imgResult, markers);
    Mat mark;
    markers.convertTo(mark, CV_8U);
    bitwise_not(mark, mark);
    //    imshow("Markers_v2", mark); // uncomment this if you want to see how the mark
    // image looks like at that point
    // Generate random colors
    vector<Vec3b> colors;
    for (size_t i = 0; i < contours.size(); i++) {
        int b = theRNG().uniform(0, 256);
        int g = theRNG().uniform(0, 256);
        int r = theRNG().uniform(0, 256);
        colors.push_back(Vec3b((uchar) b, (uchar) g, (uchar) r));
    }
    // Create the result image
    Mat dst = Mat::zeros(markers.size(), CV_8UC3);
    // Fill labeled objects with random colors
    for (int i = 0; i < markers.rows; i++) {
        for (int j = 0; j < markers.cols; j++) {
            int index = markers.at<int>(i, j);
            if (index > 0 && index <= static_cast<int>(contours.size())) {
                dst.at<Vec3b>(i, j) = colors[index - 1];
            }
        }
    }
    // Visualize the final image
    namedWindow("Final Result", 0);
    imshow("Final Result", dst);
    waitKey(0);

    IntensityImage_ = dst;


    return markers;
}






vector<Mat> SepDisegni::split_merge(Mat watershededImage) {


    /* std::map<int32_t, int32_t> labelCounts;
    for( int y = 0; y < watershededImage.rows; y++) {
        for( int x = 0; x < watershededImage.cols; x++) {
            auto label = watershededImage.at<int32_t>(y,x);
            if(labelCounts.count(label) > 0) {
                labelCounts[label] += 1;
            } else {
                labelCounts[label] = 1;
            }
        }
    } */

    //Finding Boundary Boxes
    static const int INT_MINI = std::numeric_limits<int>::min();
    static const int INT_MAXI = std::numeric_limits<int>::max();

    struct MinMax {
        cv::Point min{INT_MAXI, INT_MAXI};
        cv::Point max{INT_MINI, INT_MINI};
    };

    //Finding Maximum and Minimum of a subregion
    std::map<int32_t, MinMax> extremeFinder;
    for( int y = 0; y < watershededImage.rows; y++) {
        for( int x = 0; x < watershededImage.cols; x++) {

            //get label
            auto label = watershededImage.at<int32_t>(y,x);

            //Labels that are skipped because they aren't a part of the subregion
            if(label == -1 || label == 255) { continue; }

            //Make a new minmax if we don't have one for this label
            if(extremeFinder.count(label) == 0) {
                extremeFinder[label] = MinMax();
                continue;
            }

            //This finds the max (top-left point) and min (bottom-right point) for each subregion
            if (x < extremeFinder[label].min.x ) { extremeFinder[label].min.x = x; };
            if (y < extremeFinder[label].min.y ) { extremeFinder[label].min.y = y; };
            if (x > extremeFinder[label].max.x ) { extremeFinder[label].max.x = x; };
            if (y > extremeFinder[label].max.y ) { extremeFinder[label].max.y = y; };




        }
    }

    //Region of Interest taking Minimum and Maximum points
    /*int height = ((extremeFinder[1].max.y) - (extremeFinder[1].min.y));
    int width = ((extremeFinder[1].max.x) - (extremeFinder[1].min.x));
    Rect region_of_interest = Rect(extremeFinder[1].min.x,extremeFinder[1].min.y, width, height);
    Mat image_roi = image_(region_of_interest).clone(); */

    vector<Mat> regionsOfInterest;
    for(auto i = extremeFinder.begin(); i != extremeFinder.end(); i++){
        int height = ((i->second.max.y) - (i->second.min.y));
        int width = ((i->second.max.x) - (i->second.min.x));
        Rect region_of_interest = Rect(i->second.min.x, i->second.min.y, width, height);
        Mat image_roi = image_(region_of_interest).clone();
        regionsOfInterest.push_back(image_roi);
    }

    return regionsOfInterest;
};







