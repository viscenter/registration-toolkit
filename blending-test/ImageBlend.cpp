#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <queue>
#include <set>
#include <algorithm>


int INF = 100000; // Ridiculously large value to represent infinity 

//WITH EACH DIFFERENT IMAGE CHANGE THESE PARAMETERS
const int COLS = 5;
int startCol = 2;
int startRow = 0;
int endCol = 2;
int endRow = 4;

//---------------------------------------------------------+
//                 Auxiliary Functions                     |
//---------------------------------------------------------+
/* Finds the X and Y coordinates for the unvisited pixel with
   the smallest difference value

   Issues for now:
   - Doing the X and Y coordinates separately; I think there is 
     probably a more effective way in which they work together, but 
     I first wanted to get it working.
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
                min = dist[i][j]; minX = j;
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
                min = dist[i][j]; minY = i;
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
    - It does work....
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
            std::cout << i << j << std::endl;
        }
    }

    //Set starting point
    distances[row][col] = 0;
    parentX[row][col] = -1;
    parentY[row][col] = -1;
    //visitedCount++;
    
    int minsCol = img1.rows;
    int minsRow = 0;

    // This loop is just to test the output
    for (int i = 0; i < img1.rows; i++){
        for (int j = 0; j < img1.cols; j++){
            std::cout << "dist " << distances[i][j]  << std::endl; 
        }
    }

    // While we havent visited all nodes AND we arent beyond the second to last row do this:
    while((visitedCount < (img1.rows * img1.cols)) /*&& (minsRow <= img1.rows - 2)*/){        
        //Get the row and col coord of the minimum distance UNVISITED pixel
        minsCol = minDistanceX(distances, isVisited, img1.rows, img1.cols);
        minsRow = minDistanceY(distances, isVisited, img1.rows, img1.cols);


        //Visit all 5 neighbors (S, E, W, SW, SE). If their distance + the connection between the current pixel
        // is smaller than the current minimum distance set to it, we update the minimum distance. 
        //-------------------------------------------------------\/-
        if(!isVisited[minsRow + 1][minsCol]){
            // Calculate squared difference between pixel in img1 and img2
            int diffS = ssdImage.at<cv::Vec4b>(minsRow+1,minsCol)[0];

            int temp = distances[minsRow][minsCol] + diffS; //temp variable to hold the updates distance
            if (temp < distances[minsRow + 1][minsCol]){
                distances[minsRow + 1][minsCol] = temp; //update if distance is smaller
                //-PARENT INFO
                parentX[minsRow + 1][minsCol] = minsRow;
                parentY[minsRow + 1][minsCol] = minsCol;
            }
        }
        
        //Functions below are the exact same but for different directions

        if(!isVisited[minsRow + 1][minsCol + 1]){
            int diffSE = ssdImage.at<cv::Vec4b>(minsRow+1,minsCol+1)[0];

            int temp = distances[minsRow][minsCol] + diffSE;
            if (temp < distances[minsRow + 1][minsCol + 1]){
                distances[minsRow + 1][minsCol + 1] = temp;
                //-PARENT INFO
                parentX[minsRow + 1][minsCol + 1] = minsRow;
                parentY[minsRow + 1][minsCol + 1] = minsCol;
            }
        }

        if(!isVisited[minsRow + 1][minsCol - 1]){
            int diffSW = ssdImage.at<cv::Vec4b>(minsRow+1,minsCol-1)[0];

            int temp = distances[minsRow][minsCol] + diffSW;
            if (temp < distances[minsRow + 1][minsCol - 1]){
                distances[minsRow + 1][minsCol - 1] = temp;
                //-PARENT INFO
                parentX[minsRow + 1][minsCol - 1] = minsRow;
                parentY[minsRow + 1][minsCol - 1] = minsCol;
            }
        }

        if(!isVisited[minsRow][minsCol + 1]){
            int diffE = ssdImage.at<cv::Vec4b>(minsRow,minsCol+1)[0];

            int temp = distances[minsRow][minsCol] + diffE;
            if (temp < distances[minsRow][minsCol + 1]){
                distances[minsRow][minsCol + 1] = temp;
                //-PARENT INFO
                parentX[minsRow][minsCol + 1] = minsRow;
                parentY[minsRow][minsCol + 1] = minsCol;
            }
        }

        if(!isVisited[minsRow][minsCol - 1]){
            int diffW = ssdImage.at<cv::Vec4b>(minsRow,minsCol-1)[0];

            int temp = distances[minsRow][minsCol] + diffW;
            if (temp < distances[minsRow][minsCol - 1]){
                distances[minsRow][minsCol - 1] = temp;
                //-PARENT INFO
                parentX[minsRow][minsCol - 1] = minsRow;
                parentY[minsRow][minsCol - 1] = minsCol;
            }
        }
        //------------------------------------------------/\-

        isVisited[minsRow][minsCol] = true; //mark current pixel as visited
        visitedCount++;
    } 

    // XX and YY are values to test, for some reason they "work" (not always), IDK WHY x = 25 and y = 40
    int pathCol = endCol;
    int pathRow = endRow;
    
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

        pathRow = parentX[pathRow][pathCol];
        pathCol = parentY[temp][pathCol];
        
    } 
    
}


cv::Mat SSD(int rows, int cols, cv::Mat img1, cv::Mat img2, cv::Mat ssdImage) {
    int squaredDif;
    int first;
    int second;
    unsigned char value;
    cv::Vec3b black = img1.at<cv::Vec3b>(0,0);

    for(int i=0; i<rows; i++) {
        for(int j=0; j<(cols); j++) { //runs for half the image to not waste time

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

            for(int k = 0; k < 3; k++){
                ssdImage.at<cv::Vec4b>(i,j)[k] = value;
            }

            //show only the overlapping secitons (0 = alpha, 255 = opaque)
            if(!((img1.at<cv::Vec3b>(i,j) != black) && (img2.at<cv::Vec3b>(i,j) != black))) {
                ssdImage.at<cv::Vec4b>(i,j)[3] = 0;
            } 
            else ssdImage.at<cv::Vec4b>(i,j)[3] = 255; //visable

            std::cout << " sqrd dif " << squaredDif << std::endl;  
        }
    }
    return ssdImage;
}

int main()
{

    std::string image_path1 = "../../../Test_Image_1.tif";
    cv::Mat img1 = cv::imread(image_path1);
    if(img1.empty())
    {
        std::cout << "Could not read the image: " << image_path1 << std::endl;
        return 1;
    }

    std::string image_path2 = "../../../Test_Image_2.tif";
    cv::Mat img2 = cv::imread(image_path2);
    if(img2.empty())
    {
        std::cout << "Could not read the image: " << image_path2 << std::endl;
        return 1;
    }

    cv::Mat ssdImage = img1.clone();
    cvtColor(ssdImage, ssdImage, cv::COLOR_BGR2BGRA); //adding in alpha channel
    ssdImage.setTo(cv::Scalar(255,255,255,255)); //setting everything to white
    //computing difference image
    ssdImage = SSD(img1.rows, img1.cols, img1, img2, ssdImage);

    cv::Mat outputImage = ssdImage.clone();
    outputImage.setTo(cv::Scalar(0,0,0,255)); //Set everything to black (for now) because it is easier to throw in photoshop and do the "lighten" filter with the difference image
    minErrorBoundaryCutVertical(startRow, startCol, outputImage, img2, img1, ssdImage);

    // second argument: image to be shown(Mat object)
    //imshow("output", ssdImage);
    //cv::waitKey(0);

    imwrite("../../../output.png", outputImage);
   
   
    return 0;
}
