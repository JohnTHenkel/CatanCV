#include "FindBoardLocs.h"

vector<Point2f> findBoardLocs(const Mat& image){
vector<vector<Point>> boardContour=findGameBoard(image);
drawContours(image,boardContour,0,Scalar(0,0,255),2,8);
RotatedRect rect = minAreaRect(boardContour.at(0));
Point2f pt[4];
rect.points(pt);
for( int j = 0; j < 4; j++ ){
	line( image, pt[j], pt[(j+1)%4], Scalar(0,255,0), 3, 8 );
}

vector<Point2f> templatePt(4);
vector<Point2f> scenePt={pt[0],pt[1],pt[2],pt[3]};
templatePt.at(1) = Point2f(404,76);
templatePt.at(2) = Point2f(1470,76);
templatePt.at(3) = Point2f(1470,1000);
templatePt.at(0) = Point2f(404,1000);
Mat tform = findHomography(templatePt, scenePt,RANSAC);
vector<Point2f> templateCenters(19);
templateCenters.at(0) = Point2f(783,266);
templateCenters.at(1) = Point2f(943,266);
templateCenters.at(2) = Point2f(1103,266);
templateCenters.at(3) = Point2f(703,406);
templateCenters.at(4) = Point2f(863,406);
templateCenters.at(5) = Point2f(1022,406);
templateCenters.at(6) = Point2f(1182,406);
templateCenters.at(7) = Point2f(628,545);
templateCenters.at(8) = Point2f(783,545);
templateCenters.at(9) = Point2f(943,545);
templateCenters.at(10) = Point2f(1103,545);
templateCenters.at(11) = Point2f(1262,545);
templateCenters.at(12) = Point2f(704,683);
templateCenters.at(13) = Point2f(865,683);
templateCenters.at(14) = Point2f(1022,683);
templateCenters.at(15) = Point2f(1182,683);
templateCenters.at(16) = Point2f(783,821);
templateCenters.at(17) = Point2f(944,820);
templateCenters.at(18) = Point2f(1104,820);
vector<Point2f> centers(19);
perspectiveTransform(templateCenters,centers,tform);
for(int i=0; i<19; i++){
	circle(image,centers.at(i),5,Scalar(0,255,0),3,8,0);
}
imshow("centers", image);
waitKey(0);
return centers;
}
