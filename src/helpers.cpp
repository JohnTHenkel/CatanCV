#include "helpers.h"
#include "constants.h"
using namespace cv;
using namespace std;
void imshowresize(string name,const Mat& image,bool wait, bool destroy){
	Mat imageResized;
	resize(image,imageResized,Size(),SCALE,SCALE);
	namedWindow(name,WINDOW_AUTOSIZE);
	imshow(name,imageResized);
	if(wait) waitKey(0);
	if(destroy) destroyWindow(name);
}