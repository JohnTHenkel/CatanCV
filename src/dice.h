#pragma once
#include <iostream>
#include "constants.h"
#include <opencv2/opencv.hpp>
#include <cmath>
using namespace std;
using namespace cv;

int readDieRoll(Mat input);
vector<vector<Point>> findGameBoard(Mat input);
Mat transformImage(Mat input);
void mapHexagons(Mat input);