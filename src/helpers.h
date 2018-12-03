#pragma once
#include <string>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
void imshowresize(string name,const Mat& image,bool wait=true, bool destroy=true);