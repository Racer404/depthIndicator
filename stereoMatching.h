#pragma once
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp> 
#include <vector>
#include <math.h>
using namespace std;
using namespace cv;


class stereoMatching {

private:
	vector<vector<int>> left;
	vector<vector<int>> right;

public:
	int patchSize;
	int graySumMethod;
	int matchingMethod;
	float weightOfDis;
	float biasOfMinDis;
	vector<int> majorDis;
	void setPatchSize(int inputPatchSize);
	void setGraySumMethod(int inputgraySumMethod);
	void setWeightOfDis(float inputWeightOfDis);
	void setBiasOfMinDis(float inputBiasOfMinDis);
	void setMajorDis(vector<int> inputMajorDis);
	//Vec2f getSlopeAt(int patchSize, int j);
	void inputGrayScale(vector<vector<int>> inputLeft, vector<vector<int>> inputRight);
	void matchMethod(int methodIndex);
	int match(int xPixel, int yPixel);
};