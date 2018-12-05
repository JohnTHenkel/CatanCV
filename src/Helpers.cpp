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
	imageResized.release();
}
//return binary image where white represents pixels of specified color
//'r' red, 'y' yellow, 'b' blue 'o' orange
//red and blue work well for finding pieces, red and yellow work well for finding dice
//yellow has a lot of noise, hard to nullify
Mat isolateColor(const Mat& image, char color){
	Mat output;
	int hue;
	int hueRange = 10;
	int saturationFloor = 70;
	int valueFloor = 60;
	bool shouldInverse = false;
	switch(color){
		case 'r': //red
			hue = 90;
			hueRange = 8;
			shouldInverse = true;
			break;
		case 'y': //yellow
			hue = 30;
			break;
		case 'b': //blue
			hue = 105;
			hueRange = 8;
			valueFloor = 30;
			break;
		case 'o': //orange
			hue = 30;
			break;
		case 't':
			hueRange = 20;
			saturationFloor = 60;
			valueFloor = 50;
			hue=90;
			break;
		default:
			cout << "isolateColor, invalid color "<< color<<"\n";
			break;
	}
	if (image.channels() == 3){
		Mat imageInv;
		Mat imageHSV;
		if(shouldInverse){
			imageInv =  ~image;
		}
		else{
			imageInv = image;
		}
		cvtColor(imageInv, imageHSV, COLOR_BGR2HSV);
		inRange(imageHSV, Scalar(hue-hueRange,saturationFloor,valueFloor), Scalar(hue+hueRange,255,255), output);
	}
	else{
		output = image;
	}
	return output;
}
vector<vector<cv::Point>> findGameBoard(const Mat& input){
	
	Mat imageBW=~isolateColor(input,'t');
	vector<vector<Point>>boardContour;
	vector<vector<Point>> allContours;

	findContours(imageBW,allContours,RETR_CCOMP,CHAIN_APPROX_NONE);
	boardContour.resize(1);
	for (size_t i=0;i<allContours.size();i++){
		double area=contourArea(allContours.at(i));
		
		if((area>BOARD_AREA_MIN_THRESHOLD)&&(area<BOARD_AREA_MAX_THRESHOLD)){
			boardContour.at(0)=allContours.at(i);
		}
	}
	return boardContour;
}
