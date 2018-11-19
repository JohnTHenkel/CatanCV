#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
const float DICE_RADIUS_MAX_THRESHOLD=12.0;
const float DICE_RADIUS_MIN_THRESHOLD=2.0;

int readDieRoll(Mat diceCorners);
int main(int argc, char* argv[]){
	//Mat image = imread("TestPhotos/Dice6.png");
	Mat image = imread("TestPhotos/2018-11-07-162752.jpg");
	if(image.empty()){
		cerr << "Failed to read input image"<<endl;
		exit(EXIT_FAILURE);
	}
	resize(image,image,Size(),.5,.5);
	cout<< "Dice roll: "<<readDieRoll(image)<<endl;

	
	

	return EXIT_SUCCESS;
}
int readDieRoll(Mat diceCorners){
	Mat imageGray;
	Mat imageBW;

	if(diceCorners.channels()==3){
		Mat imageHSV;
		Mat invImage=~diceCorners;
		cvtColor(invImage,imageHSV,COLOR_BGR2HSV);
		inRange(imageHSV,Scalar(80,70,50),Scalar(100,255,255),imageBW);
	
		//Convert BGR image to grayscale
		//cvtColor(diceCorners,imageGray,COLOR_BGR2GRAY);

		//Convert grayscale image to binary with Otsu's method
		//threshold(imageGray,imageBW,0,255,THRESH_OTSU);
		imshow("BW",imageBW);
	}
	else{
		imageBW=diceCorners;
	}
	//Create a structuring element and perform a morphological opening 
	//on the binary image.
	Mat structuringElement=getStructuringElement(MORPH_ELLIPSE,Size(2,2));
	Mat imageOpen;
	morphologyEx(imageBW,imageOpen,MORPH_OPEN,structuringElement);
	
	//Find the contours in the opened binary image
	vector<vector<cv::Point>> contours;
	findContours(imageOpen,contours,RETR_CCOMP,CHAIN_APPROX_NONE);
	
	//Remove contours that are too large, leaving only the contours created 
	//by the dice
	vector<float> radius(contours.size());
	vector<Point2f> center(contours.size());
	for (size_t i=contours.size()-1;i!=-1;i--){
		minEnclosingCircle(contours.at(i),center.at(i),radius.at(i));
		//cout<<radius.at(i)<<endl;
		if(radius.at(i)>DICE_RADIUS_MAX_THRESHOLD||radius.at(i)<DICE_RADIUS_MIN_THRESHOLD){
			contours.erase(contours.begin()+i);
			radius.erase(radius.begin()+i);
			center.erase(center.begin()+i);
		}
	}

	drawContours(diceCorners,contours,-1,Scalar(0,255,0),2,8);
	imshow("All contours",diceCorners);
	waitKey(0);
	imwrite("dice6out.png",diceCorners);

	return (int)contours.size();
}
