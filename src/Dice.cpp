#include "Dice.h"
#include "Helpers.h"
using namespace cv;
using namespace std;
int readDieRoll(Mat input){
	Mat imageClose=transformImage(input);
	//Find the contours in the opened binary image
	vector<vector<Point>> contours;
	findContours(imageClose,contours,RETR_CCOMP,CHAIN_APPROX_NONE);
	
	//boardContour should contain the single contour representing the hexagonal game board.
	vector<vector<Point>> boardContour=findGameBoard(input);
	
	if(boardContour.at(0).empty()) return -1;
	drawContours(input,boardContour,-1,Scalar(0,255,0),2,8);

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
						
				double polyTest=pointPolygonTest(boardContour.at(0),jCenter,false);
				if(polyTest<0){
					newContours.push_back(contours.at(j));
					drawContours(input,outerContours,-1,Scalar(0,255,0),2,8);
				}
			
			}
		}
	}
	drawContours(input,newContours,-1,Scalar(0,255,0),2,8);

	imshow("All contours",input);
	waitKey(0);
	imwrite("output.png",input);

	return (int)newContours.size();

}





Mat transformImage(Mat input){
	Mat imageGray;
	Mat imageBW;
	
	//Convert BGR image to grayscale
	cvtColor(input,imageGray,COLOR_BGR2GRAY);

	//Convert grayscale image to binary with an adaptive method
	adaptiveThreshold(imageGray,imageBW,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,17,3);
	

	//Create a structuring element and perform a morphological opening 
	//on the binary image.
	Mat structuringElement=getStructuringElement(MORPH_ELLIPSE,Size(3,3));
	Mat imageOpen;
	Mat imageClose;
	morphologyEx(imageBW,imageOpen,MORPH_OPEN,structuringElement);
	morphologyEx(imageBW,imageClose,MORPH_CLOSE,structuringElement);
	imshowresize("ImClose",imageClose);
	return imageClose;	
}
