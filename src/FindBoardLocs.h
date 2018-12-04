#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "opencv2/xfeatures2d.hpp"
#include "Helpers.h"
#include "Constants.h"
using namespace std;
using namespace cv;

vector<Point2f> findBoardLocs(const Mat& image);
