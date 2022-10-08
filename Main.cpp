#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp> 
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include<stdio.h>
#include"stereoMatching.h"

using namespace cv;

int main() {
	String inputName = "Sculpture";

	Mat Left = imread(inputName + "/left.png");
	Mat Right = imread(inputName + "/right.png");

	vector<vector<int>> LeftGrayScale;
	vector<vector<int>> RightGrayScale;

	for (int width = 0; width < Left.cols; width++) {
		LeftGrayScale.push_back(vector<int>());
		RightGrayScale.push_back(vector<int>());
		for (int height = 0; height < Left.rows; height++) 
		{
			int leftGrayScale = Left.at<Vec3b>(height, width)[2] * 0.299 + Left.at<Vec3b>(height, width)[1] * 0.387 + Left.at<Vec3b>(height, width)[0] * 0.114;
			int rightGrayScale = Right.at<Vec3b>(height, width)[2] * 0.299 + Right.at<Vec3b>(height, width)[1] * 0.387 + Right.at<Vec3b>(height, width)[0] * 0.114;
			LeftGrayScale[width].push_back(leftGrayScale);
			RightGrayScale[width].push_back(rightGrayScale);
		}
	}

	stereoMatching sM;
	sM.inputGrayScale(LeftGrayScale, RightGrayScale);
	sM.setPatchSize(10);
	sM.matchMethod(1);
	sM.setWeightOfDis(0);
	sM.setBiasOfMinDis(0);

	//DEBUGING PART
	vector<Vec2i> disparityDataCloud;
	for (int i = 0; i < LeftGrayScale.size(); i++) {
		for (int j = 0; j < LeftGrayScale[0].size(); j++) {
			disparityDataCloud.push_back(Vec2i());
		}
	}

	//RENDERING PART
	int totalDis = 0;
	Mat depthMap = Mat(Left.rows, Left.cols, CV_8UC3, Scalar(0, 0, 0));
	for (int width = 0; width < Left.cols; width++) {
		for (int height = 0; height < Left.rows; height++) {
			int Z = width - sM.match(width, height);
			depthMap.at<Vec3b>(height, width)[0] = Z;
			depthMap.at<Vec3b>(height, width)[1] = Z;
			depthMap.at<Vec3b>(height, width)[2] = Z;
			totalDis = totalDis + Z;
		}
	}
	float averageDis = (float)totalDis / (float)(Left.cols * Left.rows);
	float a = 0;
	a = (float)128 / (float)(averageDis);
	for (int width = 0; width < Left.cols; width++) {
		for (int height = 0; height < Left.rows; height++) {
			int Z = depthMap.at<Vec3b>(height, width)[0] * a;
			if (Z > 255) {
				Z = 255;
			}
			depthMap.at<Vec3b>(height, width)[0] = Z;
			depthMap.at<Vec3b>(height, width)[1] = Z;
			depthMap.at<Vec3b>(height, width)[2] = Z;
		}
	}

	applyColorMap(depthMap, depthMap, COLORMAP_PLASMA);
	imshow("result", depthMap);

	std::ostringstream oss;
	oss <<  "Result/" << inputName << " METHOD=" << sM.matchingMethod << " PATCHSIZE=" << sM.patchSize << " WEIGHTOFDIS=" << sM.weightOfDis << "BIASOFMINDIS=" << sM.biasOfMinDis << ".png";
	std::string filePath = oss.str();
	
	imwrite(filePath, depthMap);

	waitKey();
}