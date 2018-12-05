#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include "Constants.h"
#include "Dice.h"
#include "Hexagon.h"
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "opencv2/xfeatures2d.hpp"
#include "Helpers.h"
#include "Player.h"
#include "Piece.h"
#include "FindPieces.h"
#include "Tile.h"
#include "FindCards.h"
using namespace cv;
using namespace std;



Mat isolateColor(Mat image,char color);
Mat diffImage(Mat image, Mat backgroundImage);
void detectEdges(Mat image);
void findNumbers(Mat image);


int main(int argc, char* argv[]){
	//Mat image = imread("TestPhotos/Dice6.png");
	//Mat image = imread("TestPhotos/2018-11-07-162752.jpg");
	//Mat base=imread("TestPhotos/2018-11-07-162701.jpg");
	//Mat image =imread("TestPhotos/2018-11-20-152025.jpg");
	//Mat backgroundImage = imread("TestPhotos/2018-11-07-162036.jpg");
	Mat backgroundImage = imread("TestPhotos/dice3.jpg");
	Mat image = imread("TestPhotos/dice3.jpg");
	Mat imageGameState = imread("TestPhotos/gameState2.jpg");
	if(image.empty()){
		cerr << "Failed to read input image"<<endl;
		exit(EXIT_FAILURE);
	}
	if(imageGameState.empty()){
                cerr << "Failed to read input image"<<endl;
                exit(EXIT_FAILURE);
        }
	
	//Point2f cardCenter = findCard(image,"largestArmy.jpg");



	Color largestArmy = findCardOwner(imageGameState,"largestArmy.jpg");


	//vector<Point2f> centers = findBoardLocs(backgroundImage);
	
	/*
	vector<Piece> pieces = findPieces(imageGameState);
	imageGameState = imread("TestPhotos/gameState.jpg");
	pieces = findPieces(imageGameState);
	imageGameState = imread("TestPhotos/gameState3.jpg");
        pieces = findPieces(imageGameState);
	imageGameState = imread("TestPhotos/gameState4.jpg");
        pieces = findPieces(imageGameState);
	imageGameState = imread("TestPhotos/gameState5.jpg");
        pieces = findPieces(imageGameState);
	*/




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
	
	//findTiles(backgroundImage, "largestArmy.jpg");
	
	/*
	findNumbers(image);
	image = imread("TestPhotos/gameState1.jpg");
	findNumbers(image);
	image = imread("TestPhotos/gameState2.jpg");
        findNumbers(image);
	image = imread("TestPhotos/gameState3.jpg");
        findNumbers(image);
	*/

	/*
	vector<vector<Point>> boardContour= findGameBoard(backgroundImage);
	drawContours(backgroundImage,boardContour,-1,Scalar(0,255,0),2,8);
	imshowresize("Board",backgroundImage);
	*/

	//mapHexagons(backgroundImage);
	findAllHexTiles(backgroundImage);
	return EXIT_SUCCESS;
}




void findNumbers(Mat imageColor){
	Mat image;	
	cvtColor(imageColor,image,COLOR_BGR2GRAY);
	//blur(image,image,Size(2,2));
	threshold(image,image,200,255,THRESH_BINARY);
	imshow("BW",image);
	waitKey(0);
	vector<Vec3f> circles;
	HoughCircles(image,circles, HOUGH_GRADIENT,2,image.rows/70,10,80,0,33);
	for( size_t i = 0; i < circles.size(); i++ ){
      		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      		int radius = cvRound(circles[i][2]);
      		// circle center
      		circle( imageColor, center, 3, Scalar(0,255,0), -1, 8, 0 );
      		// circle outline
      		circle( imageColor, center, radius, Scalar(0,0,255), 3, 8, 0 );
   	}
	imshow("detected circles", imageColor);
	waitKey(0);
	
	/*
	for(int number=2; number <=12; number++){
		if(number != 7){
			using namespace cv::xfeatures2d;
			string imagePath = "Templates/";
			imagePath.append(to_string(number));
			imagePath.append(".jpg");
			Mat img_object = imread(imagePath);
			cvtColor(img_scene, img_scene, COLOR_BGR2GRAY);
			cvtColor(img_object, img_object, COLOR_BGR2GRAY);
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
			const float ratio_thresh = 0.6f;
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
			//Mat tform = findHomography(obj, scene, RANSAC);
			imshow("Good Matches & Object detection", img_matches );
		    	waitKey();
			
		}
	}
	*/
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
	threshold(imageGray,imageGray,195,255,THRESH_BINARY);
        imshow("BW",imageGray);
        //waitKey(0);

	blur(imageGray, detectedEdges, Size(3,3));
	Canny(detectedEdges,detectedEdges, 30, 30*3, 3);
	vector<Vec4i> lines;
	int minLineLength = 50;
	int maxLineGap = 10;
	HoughLinesP(detectedEdges, lines, 1, CV_PI/180, 20, minLineLength, maxLineGap);
	Mat houghEdges;
	cvtColor(detectedEdges, houghEdges, COLOR_GRAY2BGR);
	for(size_t i = 0; i < lines.size(); i++){
    		line(houghEdges, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, LINE_AA);
	}
	imshow("source",image);
	imshow("detected lines", houghEdges);
	//waitKey(0);
	//convert to slope intercept form
	vector<vector<double>> mb(lines.size(), vector<double>(2));
	for(int i=0; i<lines.size(); i++){
		mb[i][0] = (double)(lines[i][1]-lines[i][3])/(double)(lines[i][0]-lines[i][2]);
		mb[i][1] = (double)(lines[i][1])-(mb[i][0]*(double)(lines[i][0]));
	}
	//find intersections
	vector<Point2f> intercepts;
	for(int i=0; i<detectedEdges.rows; i++){
		vector<int> y(lines.size());
		for(int j=0; j<lines.size(); j++){
			y[j] =(int)(mb[j][0] * (double)i + mb[j][1]);
		}
		for(int k=0; k<y.size(); k++){
			for(int l=k; l<y.size(); l++){
				if(y[k] = y[l]){
					intercepts.push_back(Point2f(k,y[k]));
				}
			}
		}
	}
	for(int i=0; i<intercepts.size(); i++){
	//	circle(detectedEdges,intercepts[i],50,Scalar(0,255,0),10,8,0);
	}
	imshow("detected intesections", detectedEdges);
	waitKey(0);
}
