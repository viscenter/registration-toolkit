#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <queue>
#include <set>
#include <algorithm>
using namespace std;


int INF = 100000; // Ridiculously large value to represent infinity 
const int COLS = 50;

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
void minErrorBoundaryCutVertical(int row, int col, cv::Mat output, cv::Mat img1, cv::Mat img2){
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
            int diffS = (img1.at<uchar>(minsCol+1,minsRow) - img2.at<uchar>(minsCol+1,minsRow));
            diffS *= diffS;

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
            int diffSE = (img1.at<uchar>(minsCol+1,minsRow + 1) - img2.at<uchar>(minsCol+1,minsRow + 1));
            diffSE *= diffSE;
            int temp = distances[minsCol][minsRow] + diffSE;
            if (temp < distances[minsCol + 1][minsRow + 1]){
                distances[minsCol + 1][minsRow + 1] = temp;
                //-PARENT INFO
                parentX[minsCol + 1][minsRow + 1] = minsCol;
                parentY[minsCol + 1][minsRow + 1] = minsRow;
            }
        }

        if(!isVisited[minsCol + 1][minsRow - 1]){
            int diffSW = (img1.at<uchar>(minsCol + 1,minsRow - 1) - img2.at<uchar>(minsCol + 1,minsRow - 1));
            diffSW *= diffSW;
            int temp = distances[minsCol][minsRow] + diffSW;
            if (temp < distances[minsCol + 1][minsRow - 1]){
                distances[minsCol + 1][minsRow - 1] = temp;
                //-PARENT INFO
                parentX[minsCol + 1][minsRow - 1] = minsCol;
                parentY[minsCol + 1][minsRow - 1] = minsRow;
            }
        }

        if(!isVisited[minsCol][minsRow + 1]){
            int diffE = (img1.at<uchar>(minsCol,minsRow + 1) - img2.at<uchar>(minsCol,minsRow + 1));
            diffE *= diffE;
            int temp = distances[minsCol][minsRow] + diffE;
            if (temp < distances[minsCol][minsRow + 1]){
                distances[minsCol][minsRow + 1] = temp;
                //-PARENT INFO
                parentX[minsCol][minsRow + 1] = minsCol;
                parentY[minsCol][minsRow + 1] = minsRow;
            }
        }

        if(!isVisited[minsCol][minsRow - 1]){
            int diffW = (img1.at<uchar>(minsCol,minsRow - 1) - img2.at<uchar>(minsCol,minsRow - 1));
            diffW *= diffW;
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

        output.at<uchar>(xx,yy) = 255; //set path in output image to be white

        xx = parentX[xx][yy];
        yy = parentY[xx][yy];
    } 
    
}



int main()
{
    std::string image_path1 = "../image1.png";
    cv::Mat img1 = cv::imread(image_path1, cv::IMREAD_GRAYSCALE);
    if(img1.empty())
    {
        std::cout << "Could not read the image: " << image_path1 << std::endl;
        return 1;
    }

    std::string image_path2 = "../image2.png";
    cv::Mat img2 = imread(image_path2, cv::IMREAD_GRAYSCALE);
    if(img2.empty())
    {
        std::cout << "Could not read the image: " << image_path2 << std::endl;
        return 1;
    }

    cv::Mat img3(img2.rows, img2.cols, CV_8UC1, cv::Scalar(0,0, 0));

    minErrorBoundaryCutVertical(0, img2.cols/2, img3, img2, img1);

    // second argument: image to be shown(Mat object)
    imshow("output", img3);
    cv::waitKey(0);

    imwrite("./output.png", img3);
   
    return 0;
}
