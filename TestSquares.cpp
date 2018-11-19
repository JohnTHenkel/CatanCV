/* Detect squares in an image.
*/
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
// Function prototypes.
std::vector< std::vector<cv::Point2f> > findSquares(cv::Mat imageInput);
int main(int argc, char* argv[])
{
printf("Hit ESC key to quit\n");
cv::VideoCapture cap(0); // open the camera
//cv::VideoCapture cap(“myVideo.avi"); // open the video file
if (!cap.isOpened()) { // check if we succeeded
printf("error - can't open the camera or video; hit any key to quit\n");
system("PAUSE");
return EXIT_FAILURE;
}
while (true) {
cv::Mat imageInput;
cap >> imageInput;
if (imageInput.empty()) break;
cv::Mat imageInputGray;
cv::cvtColor(imageInput, imageInputGray, cv::COLOR_BGR2GRAY);
std::vector<std::vector<cv::Point2f>> squares;
squares = findSquares(imageInputGray);
printf("Number of possible squares found = %d\n", squares.size());
if (squares.size() == 0) {
// Didn't find any squares. Just display the image.
cv::imshow("My Image", imageInput);
if (cv::waitKey(1) == 27) break; // hit ESC (ascii code 27) to quit
// Continue to the next iteration of the main loop.
continue;
}
for (unsigned int iSquare = 0; iSquare < squares.size(); iSquare++) {
std::vector<cv::Point2f> squareCorners = squares[iSquare];
// Draw square as a sequence of line segments.
cv::Scalar color = cv::Scalar(0, 255, 0);
for (int j = 0; j < 4; j++) {
cv::Point p1 = squareCorners[j];
cv::Point p2 = squareCorners[(j + 1) % 4];
cv::line(imageInput, p1, p2,
color,
2, // thickness
8); // line connectivity
}
// Create a list of "ortho" square corner points.
std::vector<cv::Point2f> squareOrtho;
squareOrtho.push_back(cv::Point2f(0, 0));
squareOrtho.push_back(cv::Point2f(120, 0));
squareOrtho.push_back(cv::Point2f(120, 120));
squareOrtho.push_back(cv::Point2f(0, 120));
// Find the perspective transformation that brings current marker to rectangular form.
cv::Mat H = cv::getPerspectiveTransform(squareCorners, squareOrtho);
// Transform image to get an orthophoto square image.
cv::Mat imageSquare;
cv::Size imageSquareSize(120, 120);
cv::warpPerspective(imageInputGray, imageSquare, H, imageSquareSize);
cv::imshow("Marker", imageSquare);
}
// Show the image.
cv::imshow("My Image", imageInput);
// Wait for xx ms (0 means wait until a keypress)
if (cv::waitKey(1) == 27) break; // hit ESC (ascii code 27) to quit
}
return EXIT_SUCCESS;
}
// This function tries to find black squares in the image.
// It returns a vector of squares, where each square is represented
// as a vector of four points, arranged in counter clockwise order.
std::vector< std::vector<cv::Point2f> > findSquares(cv::Mat imageInput)
{
// Convert to gray if input is color.
cv::Mat imageInputGray;
if (imageInput.channels() == 3)
cv::cvtColor(imageInput, imageInputGray, cv::COLOR_BGR2GRAY);
else
imageInputGray = imageInput;
// Do adaptive threshold ... this compares each pixel to a local
// mean of the neighborhood. The result is a binary image, where
// dark areas of the original image are now white (1's).
cv::Mat imageThresh;
adaptiveThreshold(imageInputGray,
imageThresh, // output thresholded image
255, // output value where condition met
cv::ADAPTIVE_THRESH_GAUSSIAN_C, // local neighborhood
cv::THRESH_BINARY_INV, // threshold_type - invert
31, // blockSize (any large number)
0); // a constant to subtract from mean
// Apply morphological operations to get rid of small (noise) regions
cv::Mat structuringElmt = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
cv::Mat imageOpen;
morphologyEx(imageThresh, imageOpen, cv::MORPH_OPEN, structuringElmt);
cv::Mat imageClose;
morphologyEx(imageOpen, imageClose, cv::MORPH_CLOSE, structuringElmt);
// Find contours
std::vector<std::vector<cv::Point>> contours;
std::vector<cv::Vec4i> hierarchy;
cv::findContours(
imageClose, // input image (is destroyed)
contours, // output vector of contours
hierarchy, // hierarchical representation
cv::RETR_CCOMP, // retrieve all contours
cv::CHAIN_APPROX_NONE); // all pixels of each contour
// Iterate through all the top-level contours and find squares.
std::vector< std::vector<cv::Point2f> > squares; // output squares
for (int i = 0; i < (int)contours.size(); i++) {
// Contour should be greater than some minimum area
double a = contourArea(contours[i]);
if (!(a > 100)) continue;
// Reject the ith contour if it doesn't have a child inside.
if (hierarchy[i][2] < 0) continue;
//// Check the ratio of area to perimeter squared: R = 16*A/P^2.
//// R is equal to 1 for a square.
//double P = arcLength(contours[i], true);
//double A = contourArea(contours[i]);
//if (16 * A / (P*P) < 0.75)
// continue;
// Approximate contour by a polygon.
std::vector<cv::Point> approxCurve;
// Maximum allowed distance between the original curve and its approximation.
double eps = contours[i].size() * 0.01;
cv::approxPolyDP(contours[i], approxCurve, eps, true);
// We interested only in polygons that contain only four points.
if (approxCurve.size() != 4) continue;
// Ok, I think we have a square! Create the list of corner points.
std::vector<cv::Point2f> squareCorners;
for (int j = 0; j < 4; j++)
squareCorners.push_back(cv::Point2f(approxCurve[j]));
// Sort the points in counter-clockwise order. Trace a line
// between the first and second point.
// If the third point is on the right side, then
// the points are counter-clockwise.
cv::Point v1 = squareCorners[1] - squareCorners[0];
cv::Point v2 = squareCorners[2] - squareCorners[0];
double o = (v1.x * v2.y) - (v1.y * v2.x);
if (o < 0.0)
std::swap(squareCorners[1], squareCorners[3]);
// Store the square in our list of squares.
squares.push_back(squareCorners);
}
return squares;
}