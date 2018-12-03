#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
using namespace std;
using namespace cv;

enum Tiletype {brick,forest,grain,mountain,sheep};
class Hexagon{
	int position;
	Tiletype tiltype;	
};

Point2f findSingleHexTile(const Mat& image,const string name);
vector<Hexagon> findAllHexTiles(const Mat& image);
vector<Point2f> correctCenters(const vector<Point2f> centers);

