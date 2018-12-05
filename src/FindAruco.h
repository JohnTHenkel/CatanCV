#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include "Helpers.h"
#include <vector>
using namespace std;
using namespace cv;
struct numberLocation{
public:
	int number;
	Point2d location;
};
vector<int> findArucoDiceTags(const Mat& image);
vector<numberLocation> findArucoNumberTags (const Mat& image);