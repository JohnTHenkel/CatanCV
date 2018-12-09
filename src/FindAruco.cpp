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
		vector<Vec3d> rvecs, tvecs;
		aruco::estimatePoseSingleMarkers(markerCorners, markerLength, K, distCoeffs,	rvecs, tvecs);
		std::vector<cv::Point2d> p;
		imshowresize("aruco",image,false,false);
	}
	vector<int>diceMarkers;
	for (auto & i: markerIds){
		if (i<6){
			diceMarkers.push_back(i+1);
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
			if(markerIds.at(i)>6){
				numberLocation numLoc;
				numLoc.number=markerIds.at(i);
				numLoc.location=p.at(0);
				numberLocations.push_back(numLoc);
			}
			
		}
	}
	imshowresize("aruco",image,false,false);
		
		
		return numberLocations;
}
vector<Tile> assignTileNumbers(VideoCapture& cap,vector<Tile> hexagons){
	//VideoCapture cap(CAP_NUM);
	size_t count=0;
  	vector<numberLocation> maxNumberLocations;
	Mat imCap;
    while(1){
    	cap>>imCap;
		vector<numberLocation> numberLocations =findArucoNumberTags(imCap);
		for(auto&num: numberLocations){
			for (auto &h: hexagons){
				if(h.get_number()==0){
					double distance=norm(h.get_loc()-(Point2f)num.location);
					if(distance<70){
						h.set_number(num.number-34);
						count++;
					}
				}
			}
		} 
		cout<<"Count: "<<count<<endl;
		numberLocations.clear();
		cv::waitKey(1);
		if (count==18) break; 
    }

    for(auto& h:hexagons){
   		putText(imCap,to_string(h.get_number()),h.get_loc(),FONT_HERSHEY_SIMPLEX,1,Scalar(0,0,0),2);
   	}
   	imshowresize("nums",imCap);
    return hexagons;
}