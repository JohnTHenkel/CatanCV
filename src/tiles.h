#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
using namespace std;
using namespace cv;
Point2f findSingleHexTile(const Mat& image,const string name);
vector<Point2f> findAllHexTiles(const Mat& image);
vector<Point2f> correctCenters(const vector<Point2f> centers);