#include "FindPieces.h"

//void findPieces(const Mat& image){
vector<Piece> findPieces(const Mat& image){
	Mat redImage = isolateColor(image, 'r');
	Mat blueImage = isolateColor(image, 'b');
	//imshow("red",redImage);
	//imshow("blue",blueImage);
	//waitKey(0);
	//get contour of outer edge of board
	vector<vector<Point>> boardContour=findGameBoard(image);
	//find red contours
	vector<vector<Point>> redContours;
	vector<vector<Point>> redPieceContours;
	vector<Point2f> redCentroids;
	findContours(redImage,redContours,RETR_CCOMP,CHAIN_APPROX_NONE);
	vector<vector<Point>> redConvexContours = redContours;
	for (size_t i=0;i<redContours.size();i++){
                double area=contourArea(redContours.at(i));//find area
		convexHull(redContours.at(i),redConvexContours.at(i),false);
		double convexArea=contourArea(redConvexContours.at(i));//find convex hull to find solidity
		Moments moment=moments(redContours.at(i),false);//find moment to find centroid
		Point2f centroid=Point2f(moment.m10/moment.m00,moment.m01/moment.m00);//find centroid
                if((area>PIECE_AREA_MIN_THRESHOLD)&&(area<PIECE_AREA_MAX_THRESHOLD)){//filter by area
			if((area/convexArea) >PIECE_SOLIDITY_THRESHOLD){//filter by solidity
				if(pointPolygonTest(boardContour.at(0),centroid,false)>0){//if centroid is on board
                        		redPieceContours.push_back(redContours.at(i));
					redCentroids.push_back(centroid);
				}
			}
                }
        }
	//find blue contours
	vector<vector<Point>> blueContours;
        vector<vector<Point>> bluePieceContours;
	vector<Point2f> blueCentroids;
        findContours(blueImage,blueContours,RETR_CCOMP,CHAIN_APPROX_NONE);
	vector<vector<Point>> blueConvexContours = blueContours;
        for (size_t i=0;i<blueContours.size();i++){
                double area=contourArea(blueContours.at(i));//find area
		convexHull(blueContours.at(i),blueConvexContours.at(i),false);
                double convexArea=contourArea(blueConvexContours.at(i));//find convex hull
		Moments moment=moments(blueContours.at(i),false);//find moment
		Point2f centroid=Point2f(moment.m10/moment.m00,moment.m01/moment.m00);//find centroid
                if((area>PIECE_AREA_MIN_THRESHOLD)&&(area<PIECE_AREA_MAX_THRESHOLD)){//filter by area
			if((area/convexArea) >PIECE_SOLIDITY_THRESHOLD){//filter by solidity
				if(pointPolygonTest(boardContour.at(0),centroid,false)>0){
                        		bluePieceContours.push_back(blueContours.at(i));
					blueCentroids.push_back(centroid);
				}
			}
                }
        }
	//find type
	//draw contours
	for(int i=0; i<redPieceContours.size();i++){
		drawContours(image,redPieceContours,i,Scalar(0,0,255),2, 8);
	}
	for(int i=0; i<bluePieceContours.size();i++){
		drawContours(image,bluePieceContours,i,Scalar(255,0,0),2,8);
	}
	//imshow("pieces",image);
	//waitKey(0);
	vector<Piece> output;
	for(int i=0; i<redPieceContours.size();i++){
		output.push_back(Piece(red,defineType(redPieceContours.at(i)),redCentroids.at(i)));
	}
	for(int i=0; i<bluePieceContours.size();i++){
		output.push_back(Piece(blue,defineType(bluePieceContours.at(i)),blueCentroids.at(i)));
	}
	//label image
	for(int i=0; i<output.size(); i++){
		PieceType switchCase = output.at(i).get_type();
		switch(switchCase){
			case road:
				putText(image,"road",output.at(i).get_loc(),FONT_HERSHEY_SIMPLEX,1,Scalar(0,0,0),2);
				break;
			case settlement:
				putText(image,"settlement",output.at(i).get_loc(),FONT_HERSHEY_SIMPLEX,1,Scalar(0,0,0),2);
				break;
			case city:
				putText(image,"city",output.at(i).get_loc(),FONT_HERSHEY_SIMPLEX,1,Scalar(0,0,0),2);
				break;
			default:
				break;
		}
	}
	imshowresize("pieces",image);
	waitKey(0);

	/*
	//DEBUG
	Mat drawing = image;
	vector<RotatedRect> minRect(bluePieceContours.size());
	for(int i=0; i< bluePieceContours.size();i++){
		minRect[i] = minAreaRect(bluePieceContours.at(i));
		Point2f rect_points[4];
		minRect[i].points(rect_points);
		for( int j = 0; j < 4; j++ ){
          		line( drawing, rect_points[j], rect_points[(j+1)%4], Scalar(0,255,0), 3, 8 );
     		}
	}
	imshow("rotated rects", drawing);
	waitKey(0);
	*/



	return output;
}

PieceType defineType(vector<Point> input){
	RotatedRect minRect = minAreaRect(input);
	Point2f corners[4];
	minRect.points(corners);
	double aspectRatio;
	double dim1 = minRect.size.height;
	double dim2 = minRect.size.width;
	if(dim1>dim2){aspectRatio = dim1/dim2;}
	else{aspectRatio = dim2/dim1;}
	if(aspectRatio > MIN_ASPECT_RATIO_ROAD){
		return road;
	}
	if(aspectRatio > MIN_ASPECT_RATIO_CITY){
		return city;
	}
	return settlement;
}
