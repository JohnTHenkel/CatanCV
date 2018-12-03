#include "Helpers.h"
#include "Constants.h"
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
void mapHexagons(Mat input){
	Mat imageGray;
	Mat imageBW;
	cvtColor(input,imageGray,COLOR_BGR2GRAY);
	adaptiveThreshold(imageGray,imageBW,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,3,0);
	//threshold(imageGray,imageBW,150,255,THRESH_OTSU);	
	imshowresize("BW",imageBW);
	vector<vector<Point>> allContours;
	findContours(imageBW,allContours,RETR_CCOMP,CHAIN_APPROX_NONE);
	for (size_t i=allContours.size()-1;i!=-1;i--){
		double area=contourArea(allContours.at(i));
		if((area>HEX_AREA_MAX_THRESHOLD)||(area<HEX_AREA_MIN_THRESHOLD)){
			allContours.erase(allContours.begin()+i);
		}
	}
	drawContours(input,allContours,-1,Scalar(0,255,0),2,8);
	imshowresize("contours",input);
	imshow("contours",input);
	
}