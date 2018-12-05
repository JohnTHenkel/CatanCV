//#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "opencv2/xfeatures2d.hpp"
#include "Helpers.h"
#include "Constants.h"
#include "Piece.h"
#include "FindBoardLocs.h"
using namespace std;
using namespace cv;

//void findPieces(const Mat& image);
vector<Piece> findPieces(const Mat& image);

PieceType defineType(vector<Point>);

Point2f findRobber(const Mat& image);
