#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "opencv2/xfeatures2d.hpp"
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
Mat isolateColor(Mat image,char color);
Mat diffImage(Mat image, Mat backgroundImage);
void detectEdges(Mat image);
void findTiles(Mat image, const string& name);

int main(int argc, char* argv[]){
	//Mat image = imread("TestPhotos/Dice6.png");
	Mat image = imread("TestPhotos/2018-11-07-162752.jpg");
	//Mat base=imread("TestPhotos/2018-11-07-162701.jpg");
	//Mat image =imread("TestPhotos/2018-11-20-152025.jpg");
	Mat backgroundImage = imread("TestPhotos/2018-11-07-162036.jpg");
	if(image.empty()){
		cerr << "Failed to read input image"<<endl;
		exit(EXIT_FAILURE);
	}
	
	//resize(image,image,Size(),SCALE,SCALE);
	//cout<< "Dice roll: "<<readDieRoll(image)<<endl;

	/*
	imshow("isolated red",isolateColor(image,'r'));
	imshow("isolated orange",isolateColor(image, 'o'));
	imshow("isolated yellow",isolateColor(image,'y'));
        imshow("isolated blue",isolateColor(image, 'b'));
	imshow("input image",image);
	waitKey(0);
	*/

	/*
	imshow("difference image",diffImage(image, backgroundImage));
	waitKey(0);
	*/

	//detectEdges(backgroundImage);
	
	findTiles(backgroundImage,"Brick1.jpg");

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


//return binary image where white represents pixels of specified color
//'r' red, 'y' yellow, 'b' blue 'o' orange
//red and blue work well for finding pieces, red and yellow work well for finding dice
//yellow has a lot of noise, hard to nullify
Mat isolateColor(Mat image, char color){
	Mat output;
	int hue;
	int hueRange = 10;
	int saturationFloor = 70;
	int valueFloor = 60;
	bool shouldInverse = false;
	switch(color){
		case 'r': //red
			hue = 90;
			shouldInverse = true;
			break;
		case 'y': //yellow
			hue = 30;
			break;
		case 'b': //blue
			hue = 105;
			hueRange = 5;
			break;
		case 'o': //orange
			hue = 30;
			break;
		default:
			cout << "isolateColor, invalid color "<< color<<"\n";
			break;
	}
	if (image.channels() == 3){
		Mat imageInv;
		Mat imageHSV;
		if(shouldInverse){
			imageInv =  cv::Scalar::all(255) - image;
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


//returns a mask where white represents pixels where the input image differs from the background image
//do not recommend, not as usefull as I thought It would be.
Mat diffImage(Mat image, Mat backgroundImage){
	Mat diffImage;
    	absdiff(image, backgroundImage, diffImage);

    	Mat foregroundMask = Mat::zeros(diffImage.rows, diffImage.cols, CV_8UC1);

    	float threshold = 120.0f; //increase to detect fewer changes
    	float dist;

    	for(int j=0; j<diffImage.rows; ++j){
        	for(int i=0; i<diffImage.cols; ++i)
        	{
            		Vec3b pix = diffImage.at<Vec3b>(j,i);

            		dist = (pix[0]*pix[0] + pix[1]*pix[1] + pix[2]*pix[2]);
            		dist = sqrt(dist);

            		if(dist>threshold)
            		{
                		foregroundMask.at<unsigned char>(j,i) = 255;
            		}
        	}
    	}
	//imshow("difference",diffImage);
	return foregroundMask;
}

void detectEdges(Mat image){
	Mat imageGray;
	Mat detectedEdges;
	cvtColor(image, imageGray, COLOR_BGR2GRAY);
	blur(imageGray, detectedEdges, Size(3,3));
	Canny(detectedEdges,detectedEdges, 30, 30*3, 3);
	vector<Vec4i> lines;
	int minLineLength = 170;
	int maxLineGap = 10;
	HoughLinesP(detectedEdges, lines, 1, CV_PI/180, 5, minLineLength, maxLineGap);
	Mat houghEdges;
	cvtColor(detectedEdges, houghEdges, COLOR_GRAY2BGR);
	for(size_t i = 0; i < lines.size(); i++){
    		line(houghEdges, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, LINE_AA);
	}
	imshow("source",image);
	imshow("detected lines", houghEdges);
	waitKey(0);
}

//REQUIRES xfeatures2d !!!
void findTiles(Mat img_scene,const string& name){
	using namespace cv::xfeatures2d;
	string imagePath = "Templates/";
	imagePath.append(name);
	Mat img_object = imread(imagePath);
	cvtColor(img_scene, img_scene, COLOR_BGR2GRAY);
	cvtColor(img_object, img_object, COLOR_BGR2GRAY);
	//extractChannel(img_object,img_object, 2);
	//extractChannel(img_scene,img_scene, 2);
	//resize tile template to aproximate same size as in image and blur to approximate resolution
	resize(img_object, img_object, Size(), 0.1, 0.1);
	blur(img_object, img_object, Size(4,4));
	//detect keypoints
	//int minHessian = 400;
	int minHessian = 200;
	Ptr<SURF> detector = SURF::create(minHessian);
	vector<KeyPoint> keypoints_object, keypoints_scene;
	Mat descriptors_object, descriptors_scene;
	detector->detectAndCompute(img_object, noArray(), keypoints_object, descriptors_object);
	detector->detectAndCompute(img_scene, noArray(), keypoints_scene, descriptors_scene);
	//find matches
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
    	vector< vector<DMatch> > knn_matches;
    	matcher->knnMatch( descriptors_object, descriptors_scene, knn_matches, 2 );
	//filter matches
	//const float ratio_thresh = 0.75f;
	const float ratio_thresh = 0.75f;
    	vector<DMatch> good_matches;
    	for (size_t i = 0; i < knn_matches.size(); i++)
    	{
        	if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
        	{
            		good_matches.push_back(knn_matches[i][0]);
        	}
    	}
	//draw matches
	 Mat img_matches;
    	drawMatches( img_object, keypoints_object, img_scene, keypoints_scene, good_matches, img_matches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
	//localize object
	vector<Point2f> obj;
	vector<Point2f> scene;
	for( size_t i = 0; i < good_matches.size(); i++ ){
		obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
        	scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
	}
	Mat tform = findHomography(obj, scene, RANSAC);
	vector<Point2f> obj_corners(4);
    	obj_corners[0] = Point2f(0, 0);
    	obj_corners[1] = Point2f( (float)img_object.cols, 0 );
    	obj_corners[2] = Point2f( (float)img_object.cols, (float)img_object.rows );
    	obj_corners[3] = Point2f( 0, (float)img_object.rows );
    	vector<Point2f> scene_corners(4);
    	perspectiveTransform( obj_corners, scene_corners, tform);
	line( img_matches, scene_corners[0] + Point2f((float)img_object.cols, 0),scene_corners[1] + Point2f((float)img_object.cols, 0), Scalar(0, 255, 0), 4 );
    	line( img_matches, scene_corners[1] + Point2f((float)img_object.cols, 0),scene_corners[2] + Point2f((float)img_object.cols, 0), Scalar( 0, 255, 0), 4 );
    	line( img_matches, scene_corners[2] + Point2f((float)img_object.cols, 0),scene_corners[3] + Point2f((float)img_object.cols, 0), Scalar( 0, 255, 0), 4 );
    	line( img_matches, scene_corners[3] + Point2f((float)img_object.cols, 0),scene_corners[0] + Point2f((float)img_object.cols, 0), Scalar( 0, 255, 0), 4 );
	imshow("Good Matches & Object detection", img_matches );
    	waitKey();
}
