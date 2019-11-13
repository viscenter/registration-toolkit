//
// Created by Sammy Stampley on 11/12/19.
//
#include "SepDisegni.h"



int main() {


    //TESTING PURPOSE
    Mat input = imread(samples::findFile("Shapes.jpg") , IMREAD_COLOR);

    if( input.empty() )                      // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        exit(0);
    }
    //TESTING PURPOSE

    SepDisegni a;
    a.SetImage(input);
    vector<Mat> fragmentedImage = a.Compute();

    //TESTING PURPOSE

    namedWindow("Fragmented Image", 0);
    imshow("Fragmented Image", fragmentedImage[0]);
    waitKey(0);
    //TESTING PURPOSE

    return 0;
}
