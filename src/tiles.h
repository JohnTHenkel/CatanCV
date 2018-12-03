#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
Point2f findTiles(const Mat& image,const string& name);