#include "Hexagon.h"

vector<Hexagon> findAllHexTiles(const Mat& image){
	Mat hexImage=image;
	vector<Point2f> centers;
	int radius=110;
	vector<string> templates={
//	"desert.jpg",
	"brick1.jpg",
	"brick2.jpg",
	"brick3.jpg",
	"forest1.jpg",
	"forest2.jpg",
	"forest3.jpg",
	"forest4.jpg",
	"grain1.jpg",
	"grain2.jpg",
	"grain3.jpg",
	"grain4.jpg",
	"mountain1.jpg",
	"mountain2.jpg",
	"mountain3.jpg", 
	"sheep1.jpg",
	"sheep2.jpg",
	"sheep3.jpg",
	"sheep4.jpg"
	};
	for (auto& t: templates){
		Point2f center = findSingleHexTile(image,t);
		centers.push_back(center);		
	}
	vector<Point2f> correctedCenters=correctCenters(centers);
	for(auto& p: correctedCenters){
		circle(hexImage,p,(int)radius,Scalar(0,0,255),4);
		circle(hexImage,p,10,Scalar(0,0,0),FILLED,LINE_8);
	}
	imshowresize("Final",hexImage);
	//	mapHexagons(image);
	vector<Hexagon> hexagons;
	return hexagons;
}
Point2f findSingleHexTile(const Mat& image,const string name){
	Mat img_scene_color=image;
	using namespace cv::xfeatures2d;
	string imagePath = "Templates/";
	imagePath.append(name);
	Mat img_object = imread(imagePath);
	Mat img_scene;
	cvtColor(img_scene_color, img_scene, COLOR_BGR2GRAY);
	cvtColor(img_object, img_object, COLOR_BGR2GRAY);
	//detect keypoints
	//int minHessian = 400;
	int minHessian = 80;
	if(name.substr(0,5)=="sheep") minHessian=30;
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
	imshowresize("Good Matches & Object detection", img_matches,false);
	//localize object
	vector<Point2f> obj;
	vector<Point2f> scene;
	for( size_t i = 0; i < good_matches.size(); i++ ){
		obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
        	scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
	}

	Mat tform = findHomography(obj, scene, RANSAC);
	/*
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
	*/
	vector<Point2f> obj_corners(6);
	obj_corners[0] = Point2f(53,8);
	obj_corners[1] = Point2f(151,8);
	obj_corners[2] = Point2f(200,90);
	obj_corners[3] = Point2f(152,174);
	obj_corners[4] = Point2f(57,174);
	obj_corners[5] = Point2f(6,92);
	//obj_corners from hex template
	vector<Point2f> scene_corners(5);
	perspectiveTransform(obj_corners,scene_corners,tform);
	Point2f center;
	float radius;
	minEnclosingCircle(scene_corners,center,radius);
	cout<<"Name: "<<name<<" Center: " <<center<<" Radius: "<<radius<<endl;
	

	imshowresize("new corners", img_scene_color,false);
	
	img_matches.release();
	img_scene_color.release();
	img_object.release();
	img_scene.release();
	descriptors_object.release();
	descriptors_scene.release();
	tform.release();
	keypoints_scene.clear();
	keypoints_object.clear();
	obj.clear();
	scene.clear();
	scene_corners.clear();
	obj_corners.clear();
	knn_matches.clear();

	detector.release();
	matcher.release();
	
	return(center);
}
vector<Point2f> correctCenters(vector<Point2f> centers){
	//TODO: Implement
	return centers;
}
void mapHexagons(const Mat& input){
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