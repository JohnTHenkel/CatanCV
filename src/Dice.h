#include <iostream>
#include "Constants.h"
#include <opencv2/opencv.hpp>
#include <cmath>
using namespace std;
using namespace cv;

int readDieRoll(Mat input);
vector<vector<Point>> findGameBoard(const Mat& input);
Mat transformImage(Mat input);
