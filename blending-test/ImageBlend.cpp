#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/types.hpp>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <algorithm>

#include "../core/include/rt/io/ImageIO.hpp"
#include "../core/include/rt/util/ImageConversion.hpp"

float INF = std::numeric_limits<float>::infinity();

int countSSD = 0;

struct coord {
    int x;
    int y;
};



//---------------------------------------------------------+
//                 Auxiliary Functions                     |
//---------------------------------------------------------+
/* Finds the X and Y coordinates for the unvisited pixel with
   the smallest difference value
*/

// imgContains not being used for now
bool imgContains(cv::Mat img, int y, int x){
    if(y >= img.rows || y < 0){
        return false;
    }
    else if(x >= img.cols || y < 0){
        return false;
    }
    else {
        return true;
    }
}

coord minDistanceMap(std::map<std::pair<int,int>, float> neighbors){
    float min = INF;
    coord p;
    p.x = -1;
    p.y = -1;

    for (auto const& [key, val] : neighbors){

        if(val < min){
            min = val;
            p.x = key.second;
            p.y = key.first;
        }
    }

    return p;
}


//---------------------------------------------------------+
//                Main Function - Vertical                 |
//---------------------------------------------------------+
/* Arguments:
    - row col --> Start point coordinates
    - output
    - img1
    - img2
*/
// This function finds the path and draws it
void minErrorBoundaryCutVertical(int row, int col, cv::Mat output, cv::Mat img1, cv::Mat img2, cv::Mat ssdImage){
    int visitedCount = 0;

    // Parent Arrays Store the coordinates of the previous node in the path
    cv::Mat parentY(img1.rows, img1.cols, CV_32SC1); //store in (y,x)
    cv::Mat parentX(img1.rows, img1.cols, CV_32SC1);
    cv::Mat distances (img1.rows, img1.cols, CV_32SC1);
    cv::Mat isVisited=cv::Mat::zeros(img1.rows, img1.cols, CV_8U); //any value = true, 0 = false

    std::map<std::pair<int,int>, float> neighbors;


    //Initialize the arrays
    // All distances are set to infinity
    // All pixels are set to unvisited
    for (int i = 0; i < img1.rows; i++){
        for (int j = 0; j < img1.cols; j++){
            distances.at<int>(i, j) = INF;
        }
    }


    //Set starting point
    distances.at<int>(row, col) = 0;
    parentY.at<int>(row, col) = -1; //y value
    parentX.at<int>(row, col) = -1; //x value
    neighbors.insert(std::pair<std::pair<int,int>, float>(std::pair<int,int>(row,col), 0));
    
    int minsCol = img1.rows;
    int minsRow = 0;

    coord min;


    // While we havent visited all nodes AND we arent beyond the second to last row do this:
    while((visitedCount < countSSD )/*(img1.rows * img1.cols)) && (minsRow <= img1.rows - 2)*/){        
        //Get the row and col coord of the minimum distance UNVISITED pixel
        min = minDistanceMap(neighbors);
        minsCol = min.x;
        minsRow = min.y;

        //Visit all 5 neighbors (S, E, W, SW, SE). If their distance + the connection between the current pixel
        // is smaller than the current minimum distance set to it, we update the minimum distance. 
        //-------------------------------------------------------\/-
        //SOUTH
        if(!isVisited.at<uchar>(minsRow + 1,minsCol)){
            // Calculate squared difference between pixel in img1 and img2
            int diffS = ssdImage.at<cv::Vec4b>(minsRow+1,minsCol)[0];

            int temp = distances.at<int>(minsRow,minsCol) + diffS; //temp variable to hold the updates distance
            if (temp < distances.at<int>(minsRow + 1,minsCol) && ssdImage.at<cv::Vec4b>(minsRow+1, minsCol)[3] != 0){ //and it is not outside the overlapping area (which is all alpha)
                distances.at<int>(minsRow + 1,minsCol) = temp; //update if distance is smaller
                //-PARENT INFO
                parentY.at<int>(minsRow + 1,minsCol) = minsRow;
                parentX.at<int>(minsRow + 1, minsCol) = minsCol;


                coord c;
                c.x = minsCol;
                c.y = minsRow + 1;
                neighbors.insert(std::pair<std::pair<int,int>, float>(std::pair<int,int>(c.y,c.x), temp));
            }
        }
        
        //Functions below are the exact same but for different directions
        //SOUTH EAST
        if(!isVisited.at<uchar>(minsRow + 1,minsCol + 1)){
            int diffSE = ssdImage.at<cv::Vec4b>(minsRow+1,minsCol+1)[0];

            int temp = distances.at<int>(minsRow,minsCol) + diffSE;
            if (temp < distances.at<int>(minsRow + 1,minsCol + 1) && ssdImage.at<cv::Vec4b>(minsRow+1, minsCol+1)[3] != 0){
                distances.at<int>(minsRow + 1,minsCol + 1) = temp;
                //-PARENT INFO
                parentY.at<int>(minsRow + 1,minsCol + 1) = minsRow;
                parentX.at<int>(minsRow + 1,minsCol + 1) = minsCol;

                coord c;
                c.x = minsCol + 1;
                c.y = minsRow + 1;
                neighbors.insert(std::pair<std::pair<int,int>, float>(std::pair<int,int>(c.y,c.x), temp));
            }
        }
        //SOUTH WEST
        if(!isVisited.at<uchar>(minsRow + 1,minsCol - 1)){
            int diffSW = ssdImage.at<cv::Vec4b>(minsRow+1,minsCol-1)[0];

            int temp = distances.at<int>(minsRow,minsCol) + diffSW;
            if (temp < distances.at<int>(minsRow + 1,minsCol - 1) && ssdImage.at<cv::Vec4b>(minsRow+1, minsCol-1)[3] != 0){
                distances.at<int>(minsRow + 1,minsCol - 1) = temp;
                //-PARENT INFO
                parentY.at<int>(minsRow + 1,minsCol - 1) = minsRow;
                parentX.at<int>(minsRow + 1,minsCol - 1) = minsCol;

                coord c;
                c.x = minsCol - 1;
                c.y = minsRow + 1;
                neighbors.insert(std::pair<std::pair<int,int>, float>(std::pair<int,int>(c.y,c.x), temp));
            }
        }
        //EAST
        if(!isVisited.at<uchar>(minsRow,minsCol + 1)){
            int diffE = ssdImage.at<cv::Vec4b>(minsRow,minsCol+1)[0];

            int temp = distances.at<int>(minsRow,minsCol) + diffE;
            if (temp < distances.at<int>(minsRow,minsCol + 1) && ssdImage.at<cv::Vec4b>(minsRow, minsCol+1)[3] != 0){
                distances.at<int>(minsRow,minsCol + 1) = temp;
                //-PARENT INFO
                parentY.at<int>(minsRow,minsCol + 1) = minsRow;
                parentX.at<int>(minsRow,minsCol + 1) = minsCol;

                coord c;
                c.x = minsCol + 1;
                c.y = minsRow;
                neighbors.insert(std::pair<std::pair<int,int>, float>(std::pair<int,int>(c.y,c.x), temp));
            }
        }
        //WEST
        if(!isVisited.at<uchar>(minsRow,minsCol - 1)){
            int diffW = ssdImage.at<cv::Vec4b>(minsRow,minsCol-1)[0];

            int temp = distances.at<int>(minsRow,minsCol) + diffW;
            if (temp < distances.at<int>(minsRow,minsCol - 1) && ssdImage.at<cv::Vec4b>(minsRow, minsCol-1)[3] != 0){
                distances.at<int>(minsRow,minsCol - 1) = temp;
                //-PARENT INFO
                parentY.at<int>(minsRow,minsCol - 1) = minsRow;
                parentX.at<int>(minsRow,minsCol - 1) = minsCol;

                coord c;
                c.x = minsCol + 1;
                c.y = minsRow;
                neighbors.insert(std::pair<std::pair<int,int>, float>(std::pair<int,int>(c.y,c.x), temp));
            }
        }
        //------------------------------------------------/\-

        isVisited.at<uchar>(minsRow,minsCol) = 1; //mark current pixel as visited
        neighbors.erase(std::pair<int, int>(minsRow, minsCol));
        visitedCount++;
    } 

   
    int pathCol = img1.cols/2;
    int pathRow = img1.rows - 1;

    std::cout << parentY.at<int>(25,25) << " - " << parentX.at<int>(25,25)<< std::endl;
    
    while((pathCol != -1) && (pathRow != -1)){
        /* Once it works this adds the two image halves


        for(int j = 0; j < parentX[i][j]; j++){
            output.at<uchar>(i,i) = img1.at<uchar>(i,i);
        }

        for(int j = newCol; j < (img2.cols); j++){
            output.at<uchar>(newRow,i) = img2.at<uchar>(newRow,i);
        } */
        
        
        output.at<cv::Vec4b>(pathRow, pathCol)[1] = 255; //set path in output image to be white
        int temp = pathRow;

        pathRow = parentY.at<int>(pathRow,pathCol);
        pathCol = parentX.at<int>(temp,pathCol);
    } 
    
}

//---------------------------------------------------------+
//              Main Function - Horizontal                 |
//---------------------------------------------------------+
/* Arguments:
    - row col --> Start point coordinates
    - output
    - img1
    - img2
*/

void minErrorBoundaryCutHorizontal(int row, int col, cv::Mat output, cv::Mat img1, cv::Mat img2, cv::Mat ssdImage, int endRow, int endCol){
    int visitedCount = 0;

    // Parent Arrays Store the coordinates of the previous node in the path
    // Would be better if implemented as a single array that contains both X and Y
    int colRange = endCol - col;

    cv::Mat parentY(img1.rows, colRange, CV_32SC1); //store in (y,x)
    cv::Mat parentX(img1.rows, colRange, CV_32SC1);
    cv::Mat distances (img1.rows, colRange, CV_32SC1);
    cv::Mat isVisited=cv::Mat::zeros(img1.rows, colRange, CV_8U); //any value = true, 0 = false

    std::map<std::pair<int,int>, float> neighbors;
    

    std::cout << countSSD << std::endl;


    //Initialize the arrays
    // All distances are set to infinity
    // All pixels are set to unvisited
    for (int i = 0; i < img1.rows; i++){
        for (int j = 0; j < img1.cols; j++){
            distances.at<int>(i, j) = INF;
        }
    }


    //Set starting point
    distances.at<int>(row, col) = 0;
    parentY.at<int>(row, col) = -1; //y value
    parentX.at<int>(row, col) = -1; //x value
    neighbors.insert(std::pair<std::pair<int,int>, float>(std::pair<int,int>(row,col), 0));
    
    int minsCol = img1.rows;
    int minsRow = 0;

    coord min;


    // While we havent visited all nodes AND we arent beyond the second to last row do this:
    while((visitedCount < countSSD )/*(img1.rows * img1.cols)) && (minsRow <= img1.rows - 2)*/){        
        //Get the row and col coord of the minimum distance UNVISITED pixel
        min = minDistanceMap(neighbors);
        minsCol = min.x;
        minsRow = min.y;
        //Visit all 5 neighbors (S, E, W, SW, SE). If their distance + the connection between the current pixel
        // is smaller than the current minimum distance set to it, we update the minimum distance. 
        //-------------------------------------------------------\/-
        
        //Functions below are the exact same but for different directions
        //SOUTH
        if(!isVisited.at<uchar>(minsRow + 1,minsCol)){
            // Calculate squared difference between pixel in img1 and img2
            int diffS = ssdImage.at<cv::Vec4b>(minsRow+1,minsCol)[0];

            int temp = distances.at<int>(minsRow,minsCol) + diffS; //temp variable to hold the updates distance
            if (temp < distances.at<int>(minsRow + 1,minsCol) && ssdImage.at<cv::Vec4b>(minsRow+1, minsCol)[3] != 0){ //and it is not outside the overlapping area (which is all alpha)
                distances.at<int>(minsRow + 1,minsCol) = temp; //update if distance is smaller
                //-PARENT INFO
                parentY.at<int>(minsRow + 1,minsCol) = minsRow;
                parentX.at<int>(minsRow + 1, minsCol) = minsCol;


                coord c;
                c.x = minsCol;
                c.y = minsRow + 1;
                neighbors.insert(std::pair<std::pair<int,int>, float>(std::pair<int,int>(c.y,c.x), temp));
            }
        }

        //SOUTH EAST
        if(!isVisited.at<uchar>(minsRow + 1,minsCol + 1)){
            int diffSE = ssdImage.at<cv::Vec4b>(minsRow+1,minsCol+1)[0];

            int temp = distances.at<int>(minsRow,minsCol) + diffSE;
            if (temp < distances.at<int>(minsRow + 1,minsCol + 1) && ssdImage.at<cv::Vec4b>(minsRow+1, minsCol+1)[3] != 0){
                distances.at<int>(minsRow + 1,minsCol + 1) = temp;
                //-PARENT INFO
                parentY.at<int>(minsRow + 1,minsCol + 1) = minsRow;
                parentX.at<int>(minsRow + 1,minsCol + 1) = minsCol;

                coord c;
                c.x = minsCol + 1;
                c.y = minsRow + 1;
                neighbors.insert(std::pair<std::pair<int,int>, float>(std::pair<int,int>(c.y,c.x), temp));
            }
        }
        
        //EAST
        if(!isVisited.at<uchar>(minsRow,minsCol + 1)){
            int diffE = ssdImage.at<cv::Vec4b>(minsRow,minsCol+1)[0];

            int temp = distances.at<int>(minsRow,minsCol) + diffE;
            if (temp < distances.at<int>(minsRow,minsCol + 1) && ssdImage.at<cv::Vec4b>(minsRow, minsCol+1)[3] != 0){
                distances.at<int>(minsRow,minsCol + 1) = temp;
                //-PARENT INFO
                parentY.at<int>(minsRow,minsCol + 1) = minsRow;
                parentX.at<int>(minsRow,minsCol + 1) = minsCol;

                coord c;
                c.x = minsCol + 1;
                c.y = minsRow;
                neighbors.insert(std::pair<std::pair<int,int>, float>(std::pair<int,int>(c.y,c.x), temp));
            }
        }

        //NORTH
        if(!isVisited.at<uchar>(minsRow - 1,minsCol)){
            int diffN = ssdImage.at<cv::Vec4b>(minsRow - 1,minsCol)[0];

            int temp = distances.at<int>(minsRow,minsCol) + diffN; //temp variable to hold the updates distance
            if (temp < distances.at<int>(minsRow - 1,minsCol) && ssdImage.at<cv::Vec4b>(minsRow+1, minsCol)[3] != 0){ //and it is not outside the overlapping area (which is all alpha)
                distances.at<int>(minsRow - 1,minsCol) = temp; //update if distance is smaller
                //-PARENT INFO
                parentY.at<int>(minsRow - 1,minsCol) = minsRow;
                parentX.at<int>(minsRow - 1, minsCol) = minsCol;


                coord c;
                c.x = minsCol;
                c.y = minsRow - 1;
                neighbors.insert(std::pair<std::pair<int,int>, float>(std::pair<int,int>(c.y,c.x), temp));
            }
        }

        //NORTH EAST
        if(!isVisited.at<uchar>(minsRow - 1,minsCol + 1)){
            int diffNE = ssdImage.at<cv::Vec4b>(minsRow - 1,minsCol+ 1)[0];

            int temp = distances.at<int>(minsRow,minsCol) + diffNE; //temp variable to hold the updates distance
            if (temp < distances.at<int>(minsRow - 1,minsCol+ 1) && ssdImage.at<cv::Vec4b>(minsRow+1, minsCol)[3] != 0){ //and it is not outside the overlapping area (which is all alpha)
                distances.at<int>(minsRow - 1,minsCol+ 1) = temp; //update if distance is smaller
                //-PARENT INFO
                parentY.at<int>(minsRow - 1,minsCol+ 1) = minsRow;
                parentX.at<int>(minsRow - 1, minsCol+ 1) = minsCol;


                coord c;
                c.x = minsCol;
                c.y = minsRow - 1;
                neighbors.insert(std::pair<std::pair<int,int>, float>(std::pair<int,int>(c.y,c.x), temp));
            }
        }

        
       
        //------------------------------------------------/\-

        isVisited.at<uchar>(minsRow,minsCol) = 1; //mark current pixel as visited
        neighbors.erase(std::pair<int, int>(minsRow, minsCol));
        visitedCount++;
    } 

   
    int pathCol = endCol;
    int pathRow = endRow;

    std::cout << parentY.at<int>(25,25) << " - " << parentX.at<int>(25,25)<< std::endl;
    
    while((pathCol != -1) && (pathRow != -1)){
        /* Once it works this adds the two image halves


        for(int j = 0; j < parentX[i][j]; j++){
            output.at<uchar>(i,i) = img1.at<uchar>(i,i);
        }

        for(int j = newCol; j < (img2.cols); j++){
            output.at<uchar>(newRow,i) = img2.at<uchar>(newRow,i);
        } */
        
        std::cout << "CHECk" << std::endl;
        
        output.at<cv::Vec4b>(pathRow, pathCol)[1] = 255; //set path in output image to be white
        int temp = pathRow;

        pathRow = parentY.at<int>(pathRow,pathCol);
        pathCol = parentX.at<int>(temp,pathCol);
        
    } 

     std::cout << "CHECk after 2nd while loop" << std::endl;
    
}


cv::Mat SSD(int rows, int cols, cv::Mat img1Gray, cv::Mat img2Gray, cv::Mat ssdImage) {
    float squaredDif;

    //temp value to compare in function
    cv::Vec3b black = {0,0,0};

    for(int i=0; i<rows; i++) {
        for(int j=0; j<cols; j++) { 
            //finding squared difference 
            //uchar = 8 bit image (0-255)
            //ushort = 16 bit image (0-65535)
            //float = 32 bit image (-1.18e-38 - 3.40e-38)
            squaredDif = (img1Gray.at<float>(i,j) - img2Gray.at<float>(i,j)) * (img1Gray.at<float>(i,j) - img2Gray.at<float>(i,j));


            //setting pixel value in the 4 channel ssdImage
            for(int k = 0; k < 3; k++){
                ssdImage.at<cv::Vec4f>(i,j)[k] = squaredDif; //normalized;//value;
            }

            //code to show only the overlapping secitons (0 = alpha, 255 = opaque)
            //0 = black
            //This finds the spot where both images have color (assuming the images do not have an alpha channel so the floating space is just balck)
            //and sets everything that is not whithin that space to be alpha 
            // if(!((img1.at<cv::Vec3b>(i,j) != black) && (img2.at<cv::Vec3b>(i,j) != black))) {
            //     ssdImage.at<cv::Vec4b>(i,j)[3] = 0;
            // } 
            // else {
            //     ssdImage.at<cv::Vec4b>(i,j)[3] = 255; //visable
            // }
        }
    }
    //Image needs to be viewed in Fiji to get a good idea of what it looks like
    rt::WriteImage("../../../outputSSD.tif", ssdImage);
    return ssdImage;
}

int main(int argc, char*argv[])
{
    /*
    Things needed to add for the main code:
    - user command line input structure
    - convert given images to 32 bit float upon reading them in using the quantize function
    */

    //takes in the images provided on the command line
    std::string image_path1 = argv[1];
    cv::Mat img1 = rt::ReadImage(image_path1);
    if(img1.empty())
    {
        std::cout << "Could not read the image: " << image_path1 << std::endl;
        return 1;
    }

    std::string image_path2 = argv[2];
    cv::Mat img2 = rt::ReadImage(image_path2);
    if(img2.empty())
    {
        std::cout << "Could not read the image: " << image_path2 << std::endl;
        return 1;
    }

    //saving gray scale 32 bit float versions of the image
    cv::Mat img1Gray;
    img1Gray = rt::QuantizeImage(img1, CV_32F);
    img1Gray = rt::ColorConvertImage(img1Gray, 1); 
    cv::Mat img2Gray;
    img2Gray = rt::QuantizeImage(img2, CV_32F);
    img2Gray = rt::ColorConvertImage(img2Gray, 1); 

    //making ssdImage with the demensions of the given images, and set mat to be an opaque white
    cv::Mat ssdImage(img1.rows, img1.cols, CV_32FC4, cv::Scalar(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()));
    rt::WriteImage("../../../outputSSD_noChange.tif", ssdImage);
    
    //computing difference image
    ssdImage = SSD(img1.rows, img1.cols, img1Gray, img2Gray, ssdImage);

    int startRow = 0;
    int startCol = img1.cols/2;
    
    //create the output image 
    cv::Mat outputImage(img1.rows, img1.cols, CV_32FC4, cv::Scalar(0, 0, 0, std::numeric_limits<float>::infinity())); //Set everything to black opaque image
    minErrorBoundaryCutVertical(startRow, startCol, outputImage, img2, img1, ssdImage);
    //minErrorBoundaryCutHorizontal(startRow, startCol, outputImage, img2, img1, ssdImage, 250, 250);

    //write out the output image
    imwrite("../../../output.tif", outputImage);
   
    return 0;
}
