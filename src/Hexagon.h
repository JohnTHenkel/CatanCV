#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "opencv2/xfeatures2d.hpp"
#include "Helpers.h"
#include "Constants.h"
#include "Tile.h"
using namespace std;
using namespace cv;
/*
 * use class Tile instead so it is global
enum Tiletype {brick,forest,grain,mountain,sheep};
class Hexagon{
	int position;
	Tiletype tiltype;	
};
*/
Point2f findSingleHexTile(const Mat& image,const string name);
vector<Tile> findAllHexTiles(const Mat& image);
vector<Point2f> correctCenters(const vector<Point2f> centers,const Mat& image);
void mapHexagons(const Mat& input);
