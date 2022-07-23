#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp> 
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <stdio.h>
#include <vector>
#include <math.h>
using namespace cv;
using namespace std;

// [bias] basically is the focal * baseline, check the principle for generating depth map from stereo image.
int bias = 10000;
// [patchSize] indicates the single unit of stereo matching, e.g patchSize = 2 means check 2x2 rectangle.
int patchSize = 2;
// [leftGrayScale] & [rightGrayScale] are 2d arrays store Grayscale data for left and right.
vector< vector<int> > leftGrayScale;
vector< vector<int> > rightGrayScale;

// [stereoMatching] is a function that takes input as leftPatch and generate the rightPatch that matches with it.
int* stereoMatching(int li, int lr) {
    int matchedPoint[3];
    int mininum = 999;
    for (int i = 0;i < rightGrayScale[li].size();i++) {
        int d = abs(leftGrayScale[li][lr] - rightGrayScale[li][i]);
        if (d < mininum) {
            mininum = d;
            matchedPoint[0] = li;
            matchedPoint[1] = i;
            matchedPoint[2] = mininum;
        }
    }
    return matchedPoint;
}

// Main
int main()
{
    Mat image = cv::imread("./stereoImage.png");
    Mat left = image(Range(0,image.rows),Range(0,image.cols/2));
    Mat right = image(Range(0,image.rows),Range(image.cols / 2, image.cols));

    for (int i = 0; i < left.rows; i = i + patchSize) {
        leftGrayScale.push_back(vector<int>());
        for (int j = 0; j < left.cols; j = j + 1) {
            int grayScale = 0;
            for (int k = 0;k < patchSize&&i+k<left.rows;k++) {
                for (int u = 0;u < patchSize&&j+u<left.cols;u++) {
                    grayScale = grayScale + (left.at<Vec3b>(i + k, j + u)[2]*0.299+ left.at<Vec3b>(i + k, j + u)[1]*0.387+ left.at<Vec3b>(i + k, j + u)[0]*0.114);
                }
            }
            leftGrayScale[leftGrayScale.size()-1].push_back(grayScale / (patchSize * patchSize * 3));
        }
    } 

    for (int i = 0; i < right.rows; i = i + patchSize) {
        rightGrayScale.push_back(vector<int>());
        for (int j = 0; j < right.cols; j = j + 1) {
            int grayScale = 0;
            for (int k = 0;k < patchSize && i + k < right.rows;k++) {
                for (int u = 0;u < patchSize && j+u<right.cols;u++) {
                    grayScale = grayScale + (right.at<Vec3b>(i + k, j + u)[2] * 0.299 + right.at<Vec3b>(i + k, j + u)[1] * 0.387 + right.at<Vec3b>(i + k, j + u)[0] * 0.114);
                }
            }
            rightGrayScale[rightGrayScale.size() - 1].push_back(grayScale / (patchSize * patchSize * 3));
        }
    }


    int leftPatch[2];
    leftPatch[0] = 0;
    leftPatch[1] = 0;

    int rightPatch[2];
    rightPatch[0] = 0;
    rightPatch[1] = 0;

    Mat depthMap = Mat(image.rows, image.cols/2, CV_8UC3,Scalar(0,0,0));

    for (int i = 0;i < depthMap.rows-patchSize;i=i+patchSize) {
        leftPatch[1] = i;
        for (int j = 0;j < depthMap.cols;j++) {
            leftPatch[0] = j;
            rightPatch[0]= stereoMatching(leftPatch[1] / patchSize, leftPatch[0])[1];
            rightPatch[1] = leftPatch[1];
            int Z = (bias / (abs(leftPatch[0] - rightPatch[0])+1));
            if (Z > 255) {
                Z = 255;
            }
            depthMap.at<Vec3b>(i,j)[0]= Z;
            depthMap.at<Vec3b>(i,j)[1]= Z;
            depthMap.at<Vec3b>(i,j)[2]= Z;
            for (int k = 0;k < patchSize;k++) {
                depthMap.at<Vec3b>(i+k, j)[0] = Z;
                depthMap.at<Vec3b>(i+k, j)[1] = Z;
                depthMap.at<Vec3b>(i+k, j)[2] = Z;
            }
        }
    }

    imshow("result", depthMap);

    waitKey(0);
}