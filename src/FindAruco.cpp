#include "FindAruco.h"
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
using namespace std;
using namespace cv;

double K_[3][3] =
{ { 2000, 0, 1920/2 },
{ 0, 2000, 1080/2 },
{ 0, 0, 1 } };

vector<int> findArucoDiceTags (const Mat& image){
	const float markerLength = 0.5;

	Mat K = Mat(3, 3, CV_64F, K_).clone();
	double dist_[] = { 0, 0, 0, 0, 0 };
	Mat distCoeffs = Mat(5, 1, CV_64F, dist_).clone();

	Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_ORIGINAL );
	Ptr<aruco::DetectorParameters> detectorParams = aruco::DetectorParameters::create();


	vector< int > markerIds;
	vector<vector<Point2f> > markerCorners, rejectedCandidates;
	aruco::detectMarkers(image, dictionary, markerCorners, markerIds, detectorParams, rejectedCandidates);
	if (markerIds.size() > 0) {
		aruco::drawDetectedMarkers(image, markerCorners, markerIds);
		//imshowresize("aruco",image,false,false);
	}
	vector<int>diceMarkers;
	for (auto & i: markerIds){
		if (i<6){
			diceMarkers.push_back(i);
		}
	}
	return diceMarkers;
}
vector<numberLocation> findArucoNumberTags (const Mat& image){
	const float markerLength = 0.5;
	Mat K = Mat(3, 3, CV_64F, K_).clone();
	double dist_[] = { 0, 0, 0, 0, 0 };
	Mat distCoeffs = Mat(5, 1, CV_64F, dist_).clone();
	Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_ORIGINAL );
	Ptr<aruco::DetectorParameters> detectorParams = aruco::DetectorParameters::create();
	vector< int > markerIds;
	vector<vector<Point2f> > markerCorners, rejectedCandidates;
	aruco::detectMarkers(image, dictionary, markerCorners, markerIds, detectorParams, rejectedCandidates);
	vector<numberLocation> numberLocations;

	if (markerIds.size() > 0) {
		std::vector< cv::Vec3d > rvecs, tvecs;
		// Draw all detected markers.
		cv::aruco::drawDetectedMarkers(image, markerCorners, markerIds);
		cv::aruco::estimatePoseSingleMarkers(markerCorners, markerLength ,K , distCoeffs, rvecs, tvecs); 
		for (unsigned int i = 0; i < markerIds.size(); i++) {
			
			Vec3d r = rvecs[i];
			Vec3d t = tvecs[i];
			std::vector<cv::Point3d> pointsInterest;
			pointsInterest.push_back(cv::Point3d(markerLength/2, markerLength/2, 0));
			std::vector<cv::Point2d> p;
			cv::projectPoints(pointsInterest, rvecs[i], tvecs[i], K, distCoeffs, p);
			if(markerIds.at(i)<6) break;
			numberLocation numLoc;
			numLoc.number=markerIds.at(i);
			numLoc.location=p.at(0);
			numberLocations.push_back(numLoc);
			
		}
	}
		//imshow("aruco",image);
		imshowresize("aruco",image,false,false);
		
		
		return numberLocations;
}