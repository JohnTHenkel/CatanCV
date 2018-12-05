#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "opencv2/xfeatures2d.hpp"
#include "Helpers.h"
#include "Constants.h"
#include "Enums.h"
using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

Color findCardOwner(const Mat& image, const string name);
Point2f findCard(const Mat& image,const string name);
