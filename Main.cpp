#include<opencv2/core.hpp>
#include<opencv2/opencv.hpp> 
#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<stdio.h>
#include"stereoMatching.h"

using namespace cv;

Mat Left;
Mat Right;
Mat depthMap;
Mat costMap;

int main() {
	String inputName = "Sculpture";
	int method = 1;
	int detailLevel = 4;
	/*
	vector<String> checkList;
	checkList.push_back("Adirondack");
	checkList.push_back("Backpack");
	checkList.push_back("Bicycle");
	checkList.push_back("Bridge");
	checkList.push_back("Cable");
	checkList.push_back("Minecraft");
	checkList.push_back("Pipes");
	checkList.push_back("Sculpture");
	checkList.push_back("Tardis");
	checkList.push_back("Umbrella");
	checkList.push_back("Vintage");
	for (int checkNum = 0; checkNum < checkList.size(); checkNum++) {
		inputName = checkList[checkNum];
		**/
		printf("%s\n",inputName.c_str());

		Left = imread("StereoImageDataset/" + inputName + "/left.png");
		Right = imread("StereoImageDataset/" + inputName + "/right.png");

		vector<vector<int>> LeftGrayScale;
		vector<vector<int>> RightGrayScale;

		int totalDis = 0;
		int totalCost = 0;
		depthMap = Mat(Left.rows, Left.cols, CV_8UC3, Scalar(0, 0, 0));
		costMap = Mat(Left.rows, Left.cols, CV_8UC3, Scalar(0, 0, 0));

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

		std::ostringstream oss;
		if (method == 1) {
			stereoMatching sM;
			sM.inputGrayScale(LeftGrayScale, RightGrayScale);
			sM.setPatchSize(10);
			sM.setGraySumMethod(3);
			sM.matchMethod(1);

			for (int width = 0; width < Left.cols; width++) {
				for (int height = 0; height < Left.rows; height++) {
					vector<int> p = sM.match(width, height);
					int Z = width - p[0];
					depthMap.at<Vec3b>(height, width)[0] = Z;
					depthMap.at<Vec3b>(height, width)[1] = Z;
					depthMap.at<Vec3b>(height, width)[2] = Z;

					int C = p[1];
					costMap.at<Vec3b>(height, width)[0] = C;
					costMap.at<Vec3b>(height, width)[1] = C;
					costMap.at<Vec3b>(height, width)[2] = C;

					totalDis = totalDis + Z;
					totalCost = totalCost + C;
				}
			}

			oss << "Result/" << inputName << "/Mode=GCP" << " pS=" << sM.patchSize << " gSM=" << sM.graySumMethod << ".png";
		}

		else if (method == 3) {
			stereoMatching sM;
			sM.inputGrayScale(LeftGrayScale, RightGrayScale);
			sM.setPatchSize(10);
			sM.matchMethod(1);
			sM.setGraySumMethod(3);

			stereoMatching outputsM;
			outputsM.inputGrayScale(LeftGrayScale, RightGrayScale);
			outputsM.setPatchSize(2);
			outputsM.matchMethod(3);
			outputsM.setGraySumMethod(3);
			outputsM.setWeightOfDis(0.5);

			for (int height = 0; height < Left.rows; height++) {

				vector<Vec2i> disparityArray;
				for (int width = 0; width < Left.cols; width++) {
					int Z = width - sM.match(width, height)[0];
					bool ifDisExists = false;
					for (int i = 0; i < disparityArray.size(); i++) {
						if (Z == disparityArray[i][0]) {
							disparityArray[i][1]++;
							ifDisExists = true;
							break;
						}
					}
					if (!ifDisExists) {
						disparityArray.push_back(Vec2i(Z, 0));
					}
				}


				vector<vector<int>> majorDis;
				majorDis.push_back(vector<int>());
				for (int i = 0; i < detailLevel; i++) {
					int largest = 0;
					int largestIndex = 0;
					for (int j = 0; j < disparityArray.size(); j++) {
						if (disparityArray[j][1] > largest) {
							largest = disparityArray[j][1];
							largestIndex = j;
						}
					}
					majorDis[0].push_back(disparityArray[largestIndex][0]);
					disparityArray.erase(disparityArray.begin() + largestIndex);
				}

				outputsM.setMajorDis(majorDis);

				for (int width = 0; width < Left.cols; width++) {
					int Z = width - outputsM.match(width, height)[0];
					depthMap.at<Vec3b>(height, width)[0] = Z;
					depthMap.at<Vec3b>(height, width)[1] = Z;
					depthMap.at<Vec3b>(height, width)[2] = Z;
					totalDis = totalDis + Z;
				}
			}

			oss << "Result/" << inputName << "/Mode=ADM" << " pS1=" << sM.patchSize << " pS2=" << outputsM.patchSize << " wt=" << outputsM.weightOfDis << " dtL=" << detailLevel << " gSM1=" << sM.graySumMethod << " gSM2=" << outputsM.graySumMethod << ".png";
		}

		//RENDERING
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
		//medianBlur(depthMap, depthMap, 5);
		imshow("depthMap", depthMap);
		std::string filePath = oss.str();
		imwrite(filePath, depthMap);

		float averageCost = (float)totalCost / (float)(Left.cols * Left.rows);
		a = (float)128 / (float)(averageCost);
		for (int width = 0; width < Left.cols; width++) {
			for (int height = 0; height < Left.rows; height++) {
				int C = costMap.at<Vec3b>(height, width)[0] * a;
				if (C > 255) {
					C = 255;
				}
				costMap.at<Vec3b>(height, width)[0] = C;
				costMap.at<Vec3b>(height, width)[1] = C;
				costMap.at<Vec3b>(height, width)[2] = C;
			}
		}
		imshow("costMap", costMap);

		/*
			//DEBUGING PART
			//vector<vector<Vec2i>> disparityDataCloud;
			//for (int j = 0; j < LeftGrayScale[0].size(); j++) {
			//	disparityDataCloud.push_back(vector<Vec2i>());

			stereoMatching sM;
			sM.inputGrayScale(LeftGrayScale, RightGrayScale);
			sM.setPatchSize(10);
			sM.matchMethod(1);
			sM.setWeightOfDis(0);
			sM.setBiasOfMinDis(0);

			/*stereoMatching outputsM;
			outputsM.inputGrayScale(LeftGrayScale, RightGrayScale);
			outputsM.setPatchSize(2);
			outputsM.matchMethod(3);
			outputsM.setWeightOfDis(0.5);
			outputsM.setBiasOfMinDis(0);

			vector<Vec2i> disparityArray;
			for (int width = 0; width < Left.cols; width++) {
				int Z = width - sM.match(width, 275);
				bool ifDisExists = false;
				for (int i = 0; i < disparityArray.size(); i++) {
					if (Z == disparityArray[i][0]) {
						disparityArray[i][1]++;
						ifDisExists = true;
						break;
					}
				}
				if (!ifDisExists) {
					disparityArray.push_back(Vec2i(Z, 0));
				}
			}


			vector<int> majorDis;
			for (int i = 0; i < detailLevel; i++) {
				int largest = 0;
				int largestIndex = 0;
				for (int j = 0; j < disparityArray.size(); j++) {
					if (disparityArray[j][1] > largest) {
						largest = disparityArray[j][1];
						largestIndex = j;
					}
				}
				majorDis.push_back(disparityArray[largestIndex][0]);
				disparityArray.erase(disparityArray.begin() + largestIndex);
			}

			outputsM.setMajorDis(majorDis);



	vector<Vec2i> disparityDataCloud;
	for (int i = 0; i < LeftGrayScale.size(); i++) {
			int matched = sM.match(i, 275);
			disparityDataCloud.push_back(Vec2i(i, matched));

			printf("i: %d\n", i);
			printf("matched: %d\n", matched);
	}

	disparityDataCloud.push_back(Vec2i(34,11));
	disparityDataCloud.push_back(Vec2i(50,24));
	disparityDataCloud.push_back(Vec2i(65,45));
	disparityDataCloud.push_back(Vec2i(141,116));
	disparityDataCloud.push_back(Vec2i(175,156));
	disparityDataCloud.push_back(Vec2i(204,144));
	disparityDataCloud.push_back(Vec2i(227,164));
	disparityDataCloud.push_back(Vec2i(255,191));
	disparityDataCloud.push_back(Vec2i(314,244));
	disparityDataCloud.push_back(Vec2i(382,314));
	disparityDataCloud.push_back(Vec2i(440,374));
	disparityDataCloud.push_back(Vec2i(467,425));
	disparityDataCloud.push_back(Vec2i(531,488));
	disparityDataCloud.push_back(Vec2i(91,69));
	disparityDataCloud.push_back(Vec2i(18,0));
	disparityDataCloud.push_back(Vec2i(41,19));
	disparityDataCloud.push_back(Vec2i(274,210));
	disparityDataCloud.push_back(Vec2i(241,178));
	disparityDataCloud.push_back(Vec2i(312,253));
	disparityDataCloud.push_back(Vec2i(477,448));
	disparityDataCloud.push_back(Vec2i(348,309));
	disparityDataCloud.push_back(Vec2i(332,291));
	disparityDataCloud.push_back(Vec2i(72,50));
	disparityDataCloud.push_back(Vec2i(91,69));
	disparityDataCloud.push_back(Vec2i(71,49));

		Mat result = Mat(LeftGrayScale.size(), LeftGrayScale.size(), CV_8UC3, Scalar(0, 0, 0));
		for (int i = 0; i < disparityDataCloud.size(); i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					if ((i + j) < result.rows && (i + k) < result.cols) {
						result.at<Vec3b>(disparityDataCloud[i][1] + j, disparityDataCloud[i][0] + k)[0] = 255;
						result.at<Vec3b>(disparityDataCloud[i][1] + j, disparityDataCloud[i][0] + k)[1] = 255;
						result.at<Vec3b>(disparityDataCloud[i][1] + j, disparityDataCloud[i][0] + k)[2] = 255;
					}
				}
			}
		}

		Vec4f lin;
		fitLine(disparityDataCloud, lin, DIST_L2, 0, 0.01, 0.01);

		float a;
		float b;

		a = lin[1] / lin[0];
		b = lin[3] - lin[2] * a;

		printf("a: %f\n", a);
		printf("b: %f\n", b);

		for (int i = 0; i < result.cols; i++) {
			int X = i;
			int Y = (int)(a * i + b);

			if (X > 0 && X < result.cols && Y>0 && Y < result.rows) {
				result.at<Vec3b>(Y, X)[0] = 0;
				result.at<Vec3b>(Y, X)[1] = 255;
				result.at<Vec3b>(Y, X)[2] = 255;
			}
		}

		cv::flip(result, result, 0);
		imshow("result", result);
		*/
	//}


	


	waitKey();
}