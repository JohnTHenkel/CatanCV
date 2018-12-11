#include "FindCards.h"


//ASSUME A RED AND BLUE PILE ON OUTSIDE OF BOARD
//Finds the owner of the given card (army, road) and returns the corresponding color
Color findCardOwner(const Mat& image, const string name){
	Point2f card = findCard(image, name);
	if(card.x==0&&card.y==0){
		return unassigned;
	}
	Mat redImage = isolateColor(image, 'r');
	Mat blueImage = isolateColor(image, 'b');
	vector<vector<Point>> boardContour=findGameBoard(image);
        //find red contours
        vector<vector<Point>> redContours;
        vector<vector<Point>> redPieceContours;
        vector<Point2f> redCentroids;
        findContours(redImage,redContours,RETR_CCOMP,CHAIN_APPROX_NONE);
        for (size_t i=0;i<redContours.size();i++){
		Moments moment=moments(redContours.at(i),false);//find moment to find centroid
                Point2f centroid=Point2f(moment.m10/moment.m00,moment.m01/moment.m00);//find centroid
		if(pointPolygonTest(boardContour.at(0),centroid,false)<0){
			double area=contourArea(redContours.at(i));
			if(area > MIN_PILE_AREA){
				if(norm(centroid-card)>HALF_CARD_LEN){
					redPieceContours.push_back(redContours.at(i));
					cout<<norm(centroid-card)<<endl;
				}
			}
		}
	}
	vector<vector<Point>> blueContours;
        vector<vector<Point>> bluePieceContours;
        vector<Point2f> blueCentroids;
        findContours(blueImage,blueContours,RETR_CCOMP,CHAIN_APPROX_NONE);
        for (size_t i=0;i<blueContours.size();i++){
		Moments moment=moments(blueContours.at(i),false);//find moment to find centroid
                Point2f centroid=Point2f(moment.m10/moment.m00,moment.m01/moment.m00);//find centroid
                if(pointPolygonTest(boardContour.at(0),centroid,false)<0){
                        double area=contourArea(blueContours.at(i));
                        if(area > MIN_PILE_AREA){
				if(norm(centroid-card)>HALF_CARD_LEN){
                                	bluePieceContours.push_back(blueContours.at(i));
					cout<<norm(centroid-card)<<endl;
				}
                        }
                }
        }
	for(int i=0; i<redPieceContours.size();i++){
                drawContours(image,redPieceContours,i,Scalar(0,0,255),2, 8);
        }
        for(int i=0; i<bluePieceContours.size();i++){
                drawContours(image,bluePieceContours,i,Scalar(255,0,0),2,8);
        }
	//find min distance to card
	float redMinDist = 9999;
	for(int i=0; i<redPieceContours.size(); i++){
		Moments moment=moments(redPieceContours.at(i),false);//find moment to find centroid
                Point2f centroid=Point2f(moment.m10/moment.m00,moment.m01/moment.m00);//find centroid
		float distance = norm(centroid-card);
		line( image, centroid, card, Scalar(0, 0, 255), 4 );
		if(distance < redMinDist){
			redMinDist = distance;
		}
	}
	float blueMinDist = 9999;
        for(int i=0; i<bluePieceContours.size(); i++){
                Moments moment=moments(bluePieceContours.at(i),false);//find moment to find centroid
                Point2f centroid=Point2f(moment.m10/moment.m00,moment.m01/moment.m00);//find centroid
		float distance = norm(centroid-card);
		line( image, centroid, card, Scalar(255, 0, 0), 4 );
                if(distance < blueMinDist){
			blueMinDist = distance;
		}
        }
	imshowresize("red and blue contours",image);
        waitKey(0);
	if(redMinDist < blueMinDist){
		cout<<name<<" belongs to red"<<endl;
		return red;
	}
	else if(blueMinDist < redMinDist){
		cout<<name<<" belongs to blue"<<endl;
		return blue;
	}
	return white;
}



//Performs the actual card finding of the previous function
Point2f findCard(const Mat& image,const string name){
	Mat img_scene_color=image;
	string imagePath = "Templates/";
	imagePath.append(name);
	Mat img_object = imread(imagePath);
	Mat img_scene;
	cvtColor(img_scene_color, img_scene, COLOR_BGR2GRAY);
	cvtColor(img_object, img_object, COLOR_BGR2GRAY);
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
	Mat tform;
	try{
	tform = findHomography(obj, scene, RANSAC);
	}
	catch(...){
		return Point2f(0,0);
	}
	if(tform.empty()){
		return Point2f(0,0);
	}	
	vector<Point2f> obj_corners(5);
    	obj_corners[0] = Point2f(0, 0);
    	obj_corners[1] = Point2f( (float)img_object.cols, 0 );
    	obj_corners[2] = Point2f( (float)img_object.cols, (float)img_object.rows );
    	obj_corners[3] = Point2f( 0, (float)img_object.rows );
	obj_corners[4] = Point2f((float)img_object.cols/2,(float)img_object.rows/2);
    	vector<Point2f> scene_corners(5);
    try{	
    	perspectiveTransform( obj_corners, scene_corners, tform);
    }catch(...){
    	return Point2f(0,0);
    }
	line( img_matches, scene_corners[0] + Point2f((float)img_object.cols, 0),scene_corners[1] + Point2f((float)img_object.cols, 0), Scalar(0, 255, 0), 4 );
    	line( img_matches, scene_corners[1] + Point2f((float)img_object.cols, 0),scene_corners[2] + Point2f((float)img_object.cols, 0), Scalar( 0, 255, 0), 4 );
    	line( img_matches, scene_corners[2] + Point2f((float)img_object.cols, 0),scene_corners[3] + Point2f((float)img_object.cols, 0), Scalar( 0, 255, 0), 4 );
    	line( img_matches, scene_corners[3] + Point2f((float)img_object.cols, 0),scene_corners[0] + Point2f((float)img_object.cols, 0), Scalar( 0, 255, 0), 4 );
	circle(img_matches,scene_corners[4] + Point2f((float)img_object.cols, 0),3,Scalar(255,0,0),3,8,0);
	//imwrite("Output/sift.bmp",img_matches);
	imshowresize("matches", img_matches);
	waitKey(0);
	return(scene_corners[4]);
}
