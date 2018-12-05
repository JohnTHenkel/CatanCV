#include "FindCards.h"

Point2f findCard(const Mat& image,const string name){
	Mat img_scene_color=image;
	//using namespace cv::xfeatures2d;
	string imagePath = "Templates/";
	imagePath.append(name);
	Mat img_object = imread(imagePath);
	Mat img_scene;
	cvtColor(img_scene_color, img_scene, COLOR_BGR2GRAY);
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
	imshowresize("Good Matches & Object detection", img_matches,false);
	//localize object
	vector<Point2f> obj;
	vector<Point2f> scene;
	for( size_t i = 0; i < good_matches.size(); i++ ){
		obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
        	scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
	}

	Mat tform = findHomography(obj, scene, RANSAC);
	vector<Point2f> obj_corners(5);
    	obj_corners[0] = Point2f(0, 0);
    	obj_corners[1] = Point2f( (float)img_object.cols, 0 );
    	obj_corners[2] = Point2f( (float)img_object.cols, (float)img_object.rows );
    	obj_corners[3] = Point2f( 0, (float)img_object.rows );
	obj_corners[4] = Point2f((float)img_object.cols/2,(float)img_object.rows/2);
    	vector<Point2f> scene_corners(5);
    	perspectiveTransform( obj_corners, scene_corners, tform);
	line( img_matches, scene_corners[0] + Point2f((float)img_object.cols, 0),scene_corners[1] + Point2f((float)img_object.cols, 0), Scalar(0, 255, 0), 4 );
    	line( img_matches, scene_corners[1] + Point2f((float)img_object.cols, 0),scene_corners[2] + Point2f((float)img_object.cols, 0), Scalar( 0, 255, 0), 4 );
    	line( img_matches, scene_corners[2] + Point2f((float)img_object.cols, 0),scene_corners[3] + Point2f((float)img_object.cols, 0), Scalar( 0, 255, 0), 4 );
    	line( img_matches, scene_corners[3] + Point2f((float)img_object.cols, 0),scene_corners[0] + Point2f((float)img_object.cols, 0), Scalar( 0, 255, 0), 4 );
	imshow("matches", img_matches);
	waitKey(0);
	return(obj_corners[4]);
}
