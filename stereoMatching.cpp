#include"stereoMatching.h"

void stereoMatching::setPatchSize(int inputPatchSize) {
	patchSize = inputPatchSize;
}

void stereoMatching::setWeightOfDis(float inputWeightOfDis) {
	weightOfDis = inputWeightOfDis;
}

void stereoMatching::setBiasOfMinDis(float inputBiasOfMinDis) {
	biasOfMinDis = inputBiasOfMinDis;
}

void stereoMatching::setMajorDis(vector<int> inputMajorDis) {
	majorDis = inputMajorDis;
}

void stereoMatching::inputGrayScale(vector<vector<int>> inputLeft, vector<vector<int>> inputRight) {
	left = inputLeft;
	right = inputRight;
}

void stereoMatching::matchMethod(int methodIndex) {
	matchingMethod = methodIndex;
}

//Vec2f stereoMatching::getSlopeAt(int patchSize, int j) {
//	return Vec2f(1,2);
//}

int stereoMatching::match(int xPixel, int yPixel) {
	if (matchingMethod == 1) {
		float lowestDifference = 9999999;
		int indexOfLowestDifference = 0;

		vector<int> sample;
		for (int i = 0; i < patchSize; i++) {
			if ((xPixel + i) < left.size()) {
				sample.push_back(left[xPixel + i][yPixel]);
			}
		}


		for (int i = 0; i < xPixel; i++) {
			vector<int> target;
			for (int j = 0; j < patchSize; j++) {
				if ((i + j) < xPixel) {
					target.push_back(right[i + j][yPixel]);
				}
			}
			
			if (target.size() <= sample.size()) {
				int cost = 0;
				for (int k = 0; k < target.size(); k++) {
					cost = cost + pow((sample[k] - target[k]), 2);
				}

				float difference = cost + abs(weightOfDis * abs(xPixel - i) - biasOfMinDis);

				if (difference < lowestDifference) {
					lowestDifference = difference;
					indexOfLowestDifference = i;
				}
			}
		}
		return indexOfLowestDifference;
	}













	else if (matchingMethod == 2) {
		float lowestDifference = 99999999;
		int indexOfLowestDifference = 0;
		int sampleGrayScale = 0;
		for (int i = 0; i < patchSize; i++) {
			for (int j = 0; j < patchSize; j++) {
				if (xPixel + i < left.size() && yPixel + j < left[0].size()) {
					sampleGrayScale = sampleGrayScale + left[xPixel + i][yPixel+j];
				}
			}
		}

		for (int i = 0; i < xPixel; i++) {
			int targetGrayScale = 0;
			for (int j = 0; j < patchSize; j++) {
				for (int k = 0; k < patchSize; k++) {
					if ((i + j) < xPixel && (i + k) < left[0].size()) {
						targetGrayScale = targetGrayScale + right[i + j][yPixel];
					}
				}
			}

			float difference = abs(sampleGrayScale - targetGrayScale);
			if (difference < lowestDifference) {
				lowestDifference = difference;
				indexOfLowestDifference = i;
			}
		}
		return indexOfLowestDifference;
	}










	else if (matchingMethod == 3) {

		float lowestDifference = 9999999;
		int indexOfLowestDifference = 0;

		vector<int> sample;
		for (int i = 0; i < patchSize; i++) {
			if ((xPixel + i) < left.size()) {
				sample.push_back(left[xPixel + i][yPixel]);
			}
		}

		for (int i = 0; i < xPixel; i++) {
			vector<int> target;
			for (int j = 0; j < patchSize; j++) {
				if ((i + j) < xPixel) {
					target.push_back(right[i + j][yPixel]);
				}
			}

			float denominator = 0;

			for (int majorNum = 0; majorNum < majorDis.size(); majorNum++) {
				denominator = denominator + (1 / (abs((xPixel-i) - majorDis[majorNum])+0.0001));
			}

			if (target.size() <= sample.size()) {
				int cost = 0;
				for (int k = 0; k < target.size(); k++) {
					cost = cost + pow((sample[k] - target[k]), 2);
				}

				float difference = cost + weightOfDis/denominator;

				if (difference < lowestDifference) {
					lowestDifference = difference;
					indexOfLowestDifference = i;
				}
			}
		}
		return indexOfLowestDifference;
	}
}