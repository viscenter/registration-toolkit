#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <queue>
#include <set>
#include <algorithm>


int INF = 100000; // Ridiculously large value to represent infinity 
const int COLS = 5760;

//---------------------------------------------------------+
//                 Auxiliary Functions                     |
//---------------------------------------------------------+
/* Finds the X and Y coordinates for the unvisited pixel with
   the smallest difference value

   Issues for now:
   - Doing the X and Y coordinates separately/; I think there is 
     probably a more effective way, but I first wanted to get it working
   - Requires specifying the 2D Array's size. We can change that in
     the future by using a 1D array, using vectors or other data 
     structures instead, or by improving something else. For now I left
     like this.
*/
int minDistanceX(int dist[][COLS], bool isVisited[][COLS], int rows, int cols){
    int min = INF;
    int minX = -1;

    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            if(isVisited[i][j] == false && dist[i][j] < min){
                min = dist[i][j]; minX = i;
                //std::cout << "dist x: " << min << " at i: " << i << std::endl;
            }
        }
    }

    return minX;
}

int minDistanceY(int dist[][COLS], bool isVisited[][COLS], int rows, int cols){
    int min = INF;
    int minY = -1;

    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            if(isVisited[i][j] == false && dist[i][j] < min){
                min = dist[i][j]; minY = j;
                //std::cout << "dist y " << min << std::endl;
            }
        }
    }

    return minY;
}


//---------------------------------------------------------+
//                     Main Function                       |
//---------------------------------------------------------+
/* Arguments:
    - row col --> Start point coordinates
    - output
    - img1
    - img2
/*
// This function finds the path and draws it
/* Issues for now:
    - What is X and what is Y?
*/
void minErrorBoundaryCutVertical(int row, int col, cv::Mat output, cv::Mat img1, cv::Mat img2, cv::Mat ssdImage){
    int visitedCount = 0;

    // Parent Arrays Store the coordinates of the previous node in the path
    // Would be better if implemented as a single array that contains both X and Y
    int parentX[img1.rows][COLS];
    int parentY[img1.rows][COLS];

    int distances[img1.rows][COLS]; //stores the distances of each pixel in the grid to the starting point
    bool isVisited[img1.rows][COLS];

    //Initialize the arrays
    // All distances are set to infinity
    // All pixels are set to unvisited
    for (int i = 0; i < img1.rows; i++){
        for (int j = 0; j < img1.cols; j++){
            distances[i][j] = INF;
            isVisited[i][j] = false;
        }
    }

    //Set starting point
    distances[row][col] = 0;
    parentX[row][col] = -1;
    parentY[row][col] = -1;
    //visitedCount++;
    
    int minsRow = img1.rows;
    int minsCol = 0;

    // This loop is just to test the output
    for (int i = 0; i < img1.rows; i++){
        for (int j = 0; j < img1.cols; j++){
            //std::cout << "dist " << distances[i][j]  << std::endl; 
        }
    }

    // While we havent visited all nodes AND we arent beyond the second to last row do this:
    while((visitedCount < (img1.rows * img1.cols)) && (minsCol <= img1.rows - 2)){
        
        //Get the row and col coord of the minimum distance UNVISITED pixel
        minsRow = minDistanceY(distances, isVisited, img1.rows, img1.cols);
        minsCol = minDistanceX(distances, isVisited, img1.rows, img1.cols);

        //std::cout << "( " << minsX << ", " << minsY << " )" << std::endl;

        //Visit all 5 neighbors (S, E, W, SW, SE). If their distance + the connection between the current pixel
        // is smaller than the current minimum distance set to it, we update the minimum distance. 
        //-------------------------------------------------------\/-
        if(!isVisited[minsCol + 1][minsRow]){
            // Calculate squared difference between pixel in img1 and img2
            int diffS = ssdImage.at<cv::Vec4b>(minsCol+1,minsRow)[0];

            int temp = distances[minsCol][minsRow] + diffS; //temp variable to hold the updates distance
            if (temp < distances[minsCol + 1][minsRow]){
                distances[minsCol + 1][minsRow] = temp; //update if distance is smaller
                //-PARENT INFO
                parentX[minsCol + 1][minsRow] = minsCol;
                parentY[minsCol + 1][minsRow] = minsRow;
            }
        }
        
        //Functions below are the exact same but for different directions

        if(!isVisited[minsCol + 1][minsRow + 1]){
            int diffSE = ssdImage.at<cv::Vec4b>(minsCol+1,minsRow+1)[0];

            int temp = distances[minsCol][minsRow] + diffSE;
            if (temp < distances[minsCol + 1][minsRow + 1]){
                distances[minsCol + 1][minsRow + 1] = temp;
                //-PARENT INFO
                parentX[minsCol + 1][minsRow + 1] = minsCol;
                parentY[minsCol + 1][minsRow + 1] = minsRow;
            }
        }

        if(!isVisited[minsCol + 1][minsRow - 1]){
            int diffSW = ssdImage.at<cv::Vec4b>(minsCol+1,minsRow-1)[0];

            int temp = distances[minsCol][minsRow] + diffSW;
            if (temp < distances[minsCol + 1][minsRow - 1]){
                distances[minsCol + 1][minsRow - 1] = temp;
                //-PARENT INFO
                parentX[minsCol + 1][minsRow - 1] = minsCol;
                parentY[minsCol + 1][minsRow - 1] = minsRow;
            }
        }

        if(!isVisited[minsCol][minsRow + 1]){
            int diffE = ssdImage.at<cv::Vec4b>(minsCol,minsRow+1)[0];

            int temp = distances[minsCol][minsRow] + diffE;
            if (temp < distances[minsCol][minsRow + 1]){
                distances[minsCol][minsRow + 1] = temp;
                //-PARENT INFO
                parentX[minsCol][minsRow + 1] = minsCol;
                parentY[minsCol][minsRow + 1] = minsRow;
            }
        }

        if(!isVisited[minsCol][minsRow - 1]){
            int diffW = ssdImage.at<cv::Vec4b>(minsCol,minsRow-1)[0];

            int temp = distances[minsCol][minsRow] + diffW;
            if (temp < distances[minsCol][minsRow - 1]){
                distances[minsCol][minsRow - 1] = temp;
                //-PARENT INFO
                parentX[minsCol][minsRow - 1] = minsCol;
                parentY[minsCol][minsRow - 1] = minsRow;
            }
        }
        //------------------------------------------------/\-

        isVisited[minsCol][minsRow] = true; //mark current pixel as visited
        visitedCount++;
    } 

    // XX and YY are values to test, for some reason they "work" (not always), IDK WHY x = 25 and y - 40
    int xx = 25;
    int yy = 40;

    /* testing parent arrays
    for(int i = 0; i < img1.rows; i++){
        std::cout << xx << " " << yy << std::endl;
        xx = parentX[xx][yy];
        yy = parentY[xx][yy];
    } */


    for(int i = img1.rows; i >= 0; i--){
        /* Once it works this adds the two image halves


        for(int j = 0; j < parentX[i][j]; j++){
            output.at<uchar>(i,i) = img1.at<uchar>(i,i);
        }

        for(int j = newCol; j < (img2.cols); j++){
            output.at<uchar>(newRow,i) = img2.at<uchar>(newRow,i);
        } */

        
        output.at<cv::Vec4b>(xx,yy) = (255, 255, 255); //set path in output image to be white

        xx = parentX[xx][yy];
        yy = parentY[xx][yy];
    } 
    
}


cv::Mat SSD(int rows, int cols, cv::Mat img1, cv::Mat img2, cv::Mat ssdImage) {
    int squaredDif;
    int first;
    int second;
    unsigned char value;
    cv::Vec3b black = img1.at<cv::Vec3b>(0,0);

    for(int i=0; i<rows; i++) {
        for(int j=0; j<(cols/2); j++) { //runs for half the image to not waste time

            //averaging the BGR(RGB) values for grayscale = (B+G+R)/3
            //first = (img1.at<Vec3b>(i,j)[0]+img1.at<Vec3b>(i,j)[1]+img1.at<Vec3b>(i,j)[2])/3;
            //second = (img2.at<Vec3b>(i,j)[0]+img2.at<Vec3b>(i,j)[1]+img2.at<Vec3b>(i,j)[2])/3;
            

            //weighted BGR to grayscale = 0.114B + 0.587G + 0.299R
            first = (img1.at<cv::Vec3b>(i,j)[0]*0.114)+(img1.at<cv::Vec3b>(i,j)[1]*0.587)+(img1.at<cv::Vec3b>(i,j)[2]*0.299);
            second = (img2.at<cv::Vec3b>(i,j)[0]*0.114)+(img2.at<cv::Vec3b>(i,j)[1]*0.587)+(img2.at<cv::Vec3b>(i,j)[2]*0.299);

            //finding squared difference 
            squaredDif = pow((first - second), 2);
            if(squaredDif > 255 || squaredDif < 0) {
                value = squaredDif % 256;
            }
            else { value = squaredDif; }
            
            //show only the overlapping secitons (0 = alpha, 255 = opaque)
            if(!((img1.at<cv::Vec3b>(i,j) != black) && (img2.at<cv::Vec3b>(i,j) != black))) {
                ssdImage.at<cv::Vec4b>(i,j)[3] = 0;
            }
            else ssdImage.at<cv::Vec4b>(i,j)[3] = 255; //visable
        }
    }
    return ssdImage;
}

int main()
{

    std::string image_path1 = "../../../61_16b.tif";
    cv::Mat img1 = cv::imread(image_path1);
    if(img1.empty())
    {
        std::cout << "Could not read the image: " << image_path1 << std::endl;
        return 1;
    }

    std::string image_path2 = "../../../62_16b_mk.tif";
    cv::Mat img2 = cv::imread(image_path2);
    if(img2.empty())
    {
        std::cout << "Could not read the image: " << image_path2 << std::endl;
        return 1;
    }

    cv::Mat ssdImage = img1.clone();
    cvtColor(ssdImage, ssdImage, cv::COLOR_BGR2BGRA); //adding in alpha channel
    ssdImage.setTo(cv::Scalar(255,255,255,0)); //setting everything to white
    //computing difference image
    ssdImage = SSD(img1.rows, img1.cols, img1, img2, ssdImage);

    cv::Mat outputImage = ssdImage.clone();
    int startCol = 1475;
    int startRow = 163;
    minErrorBoundaryCutVertical(startRow, startCol, outputImage, img2, img1, ssdImage);

    // second argument: image to be shown(Mat object)
    imshow("output", outputImage);
    cv::waitKey(0);

    imwrite("../../output.png", outputImage);
   
   
    return 0;
}
