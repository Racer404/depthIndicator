#pragma once
#include <vector>
#include <math.h>
using namespace std;

class stereoMatching {

private:
	vector<vector<int>> left;
	vector<vector<int>> right;

public:
	int patchSize;
	int matchingMethod;
	float weightOfDis;
	float biasOfMinDis;
	void setPatchSize(int inputPatchSize);
	void setWeightOfDis(float inputWeightOfDis);
	void setBiasOfMinDis(float inputBiasOfMinDis);
	void inputGrayScale(vector<vector<int>> inputLeft, vector<vector<int>> inputRight);
	void matchMethod(int methodIndex);
	int match(int xPixel, int yPixel);
};