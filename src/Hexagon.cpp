#include "Hexagon.h"
#include "FindAruco.h"
#include "FindBoardLocs.h"

/** Finds the hexagon tiles with surf, corrects thier centers, and returns a vector of tiles
* corrresponding to each of the smaller hexagonal game board pieces.
*/ 
vector<Tile> findAllHexTiles(const Mat& image){
	Mat hexImage=image.clone();
	vector<Point2f> centers;
	int radius=110;
	vector<string> templates={
	"sheep1.jpg",
	"sheep2.jpg",
	"sheep3.jpg",
	"sheep4.jpg",
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
	"mountain3.jpg" 
	};

	for (auto& t: templates){
		Point2f center = findSingleHexTile(image,t);
		centers.push_back(center);		
	}

	for(auto& p: centers){
		circle(hexImage,p,(int)radius,Scalar(0,0,255),4);
		circle(hexImage,p,10,Scalar(0,0,0),FILLED,LINE_8);
	}
	imshowresize("Hexes",hexImage);
	hexImage=image.clone();

	vector<Point2f> correctedCenters=correctCenters(centers,image);
	for(auto& p: correctedCenters){
		circle(hexImage,p,(int)radius,Scalar(0,0,255),4);
		circle(hexImage,p,10,Scalar(0,0,0),FILLED,LINE_8);
	}
	imshowresize("Corrected hexes",hexImage);
	vector<Tile> hexagons;

	for (int i=0;i<4;i++){
		Tile tile(sheep, correctedCenters.at(i));	
		hexagons.push_back(tile);
	}
	for (int i=4;i<7;i++){
		Tile tile(brick, correctedCenters.at(i));	
		hexagons.push_back(tile);
	}
	for (int i=7;i<11;i++){
		Tile tile(wood, correctedCenters.at(i));	
		hexagons.push_back(tile);
	}
	for (int i=11;i<15;i++){
		Tile tile(grain, correctedCenters.at(i));	
		hexagons.push_back(tile);
	}
	for (int i=15;i<18;i++){
		Tile tile(stone, correctedCenters.at(i));	
		hexagons.push_back(tile);
	}
	Tile tile(desert, correctedCenters.at(18));
	hexagons.push_back(tile);
	for(int i=0; i<hexagons.size(); i++){
		Recource switchCase = hexagons.at(i).get_recource();
		switch(switchCase){
			case wood:
				putText(hexImage,"wood",hexagons.at(i).get_loc(),FONT_HERSHEY_SIMPLEX,1,Scalar(0,0,0),2);
				break;
			case grain:
				putText(hexImage,"grain",hexagons.at(i).get_loc(),FONT_HERSHEY_SIMPLEX,1,Scalar(0,0,0),2);
				break;
			case sheep:
				putText(hexImage,"sheep",hexagons.at(i).get_loc(),FONT_HERSHEY_SIMPLEX,1,Scalar(0,0,0),2);
				break;
			case brick:
				putText(hexImage,"brick",hexagons.at(i).get_loc(),FONT_HERSHEY_SIMPLEX,1,Scalar(0,0,0),2);
				break;
			case stone:
				putText(hexImage,"stone",hexagons.at(i).get_loc(),FONT_HERSHEY_SIMPLEX,1,Scalar(0,0,0),2);
				break;
			case desert:
				putText(hexImage,"desert",hexagons.at(i).get_loc(),FONT_HERSHEY_SIMPLEX,1,Scalar(0,0,0),2);
				break;
			default:
				break;
		}
	}
	imshowresize("labels",hexImage);
	return hexagons;
}

//Finds a single hexagonal tile using SURF and returns its center
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

//Corrects the centers based on the location of the game board
vector<Point2f> correctCenters(vector<Point2f> centers,const Mat& image){
	vector<Point2f>newCenters=centers;
	Mat im=image;
	vector<Point2f> boardCenters = findBoardLocs(im);
	size_t minJ;
	double minDistance;
	for(size_t i=0;i<newCenters.size();i++){//this should iterate through boardcenters
		minDistance=999999;
		for (size_t j=boardCenters.size()-1;j!=-1;j--){
			if(norm(newCenters.at(i)-boardCenters.at(j))<minDistance){
				minDistance=norm(newCenters.at(i)-boardCenters.at(j));
				minJ=j;
			}
		}
		newCenters.at(i)=boardCenters.at(minJ);
		boardCenters.erase(boardCenters.begin()+minJ);

	}
	newCenters.push_back(boardCenters.at(0));
	return newCenters;
}

