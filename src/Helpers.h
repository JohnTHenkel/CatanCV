#pragma once
#include <string>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
void imshowresize(string name,const Mat& image,bool wait=true, bool destroy=true);
Mat isolateColor(const Mat& image, char color);
vector<vector<cv::Point>> findGameBoard(const Mat& input);