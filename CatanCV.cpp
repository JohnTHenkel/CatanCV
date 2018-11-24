#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
using namespace cv;
using namespace std;

//These values will depend on relative size of dice in frame
const double SCALE=.6;
const double DICE_AREA_MAX_THRESHOLD=800*SCALE;
const double DICE_AREA_MIN_THRESHOLD=650*SCALE;
const double DOT_AREA_MAX_THRESHOLD=30*SCALE;
const double DOT_AREA_MIN_THRESHOLD=10*SCALE;
const double BOARD_AREA_MIN_THRESHOLD=400000*SCALE;
const double BOARD_AREA_MAX_THRESHOLD=700000*SCALE;
int readDieRoll(Mat image);
int main(int argc, char* argv[]){
	//Mat image = imread("TestPhotos/Dice6.png");
	Mat image = imread("TestPhotos/2018-11-07-162752.jpg");
	//Mat base=imread("TestPhotos/2018-11-07-162701.jpg");
	//Mat image =imread("TestPhotos/2018-11-20-152025.jpg");
	if(image.empty()){
		cerr << "Failed to read input image"<<endl;
		exit(EXIT_FAILURE);
	}
	
	resize(image,image,Size(),SCALE,SCALE);
	cout<< "Dice roll: "<<readDieRoll(image)<<endl;

	
	

	return EXIT_SUCCESS;
}
int readDieRoll(Mat image){
	Mat imageGray;
	Mat imageBW;
	
	//Convert BGR image to grayscale
	cvtColor(image,imageGray,COLOR_BGR2GRAY);

	//Convert grayscale image to binary with an adaptive method
	adaptiveThreshold(imageGray,imageBW,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,15,3);
	
	imshow("BW",imageBW);

	//Create a structuring element and perform a morphological opening 
	//on the binary image.
	Mat structuringElement=getStructuringElement(MORPH_ELLIPSE,Size(3,3));
	Mat imageOpen;
	Mat imageClose;
	morphologyEx(imageBW,imageOpen,MORPH_OPEN,structuringElement);
	morphologyEx(imageBW,imageClose,MORPH_CLOSE,structuringElement);
	
	//Find the contours in the opened binary image
	vector<vector<Point>> contours;
	imshow("ImClose",imageClose);
	//imageOpen=~imageOpen;
	//imshow("~ImOpen",imageOpen);
	findContours(imageClose,contours,RETR_CCOMP,CHAIN_APPROX_NONE);
	
	//Remove contours that are too large, leaving only the contours created 
	//by the dice
	
	//boardContour should contain the single contour representing the hexagonal game board.
	vector<Point>boardContour;
	for (size_t i=0;i<contours.size();i++){
		double area=contourArea(contours.at(i));
		
		if((area>BOARD_AREA_MIN_THRESHOLD)&&(area<BOARD_AREA_MAX_THRESHOLD)){
			boardContour=contours.at(i);
		}
	}
	vector<vector<Point>>outerContours=contours;
	for (size_t i=outerContours.size()-1;i!=-1;i--){
		double area=contourArea(outerContours.at(i));
		RotatedRect box= minAreaRect(outerContours.at(i));
		double ratio=box.size.height/box.size.width;

		if((area>DICE_AREA_MAX_THRESHOLD)||(area<DICE_AREA_MIN_THRESHOLD)||ratio>1.3||ratio<0.7){
			outerContours.erase(outerContours.begin()+i);
		}
	}
	for (size_t i=contours.size()-1;i!=-1;i--){
		double area=contourArea(contours.at(i));
		if((area>DOT_AREA_MAX_THRESHOLD)||(area<DOT_AREA_MIN_THRESHOLD)){
			contours.erase(contours.begin()+i);
		}
	}
	
	//perhaps first find dice based on size and only search for contours within them
	//maybe look at sides of minimum enclosing rectangle to see if relatively equal
	vector<vector<Point>> newContours;
	float iRadius;
	float jRadius;
	Point2f iCenter;
	Point2f jCenter;
	for(size_t i=0;i<outerContours.size();i++){
		minEnclosingCircle(outerContours.at(i),iCenter,iRadius);
		for(size_t j=i+1;j<contours.size();j++){
			minEnclosingCircle(contours.at(j),jCenter,jRadius);
			float distance=sqrt(pow((iCenter.x-jCenter.x),2)+pow((iCenter.y-jCenter.y),2));
			if(distance<iRadius){				
				double polyTest=pointPolygonTest(boardContour,jCenter,false);
				if(polyTest<0){
					newContours.push_back(contours.at(j));
				}
			
			}
		}
	}
	drawContours(image,newContours,-1,Scalar(0,255,0),2,8);

	imshow("All contours",image);
	waitKey(0);
	imwrite("output.png",image);

	return (int)newContours.size();

}
