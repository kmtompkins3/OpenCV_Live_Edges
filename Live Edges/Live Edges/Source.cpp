#include <iostream>
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include <string>
#include <cv.h>
#include <Windows.h>
#include <math.h>

using namespace std;
using namespace cv;

vector<vector<Point> > contours;
vector<Vec4i> hierarchy;


RNG rng(12345);
Scalar RED(0, 0, 255);//BGR
Scalar GREEN(0, 255, 0);
Scalar WHITE(255, 255, 255);


VideoCapture cap(0); // open the default camera

long double Distance( long double point1x, long double point1y, long double point2x, long double point2y);
bool PointCompare(Point2f point1, Point2f point2);



int main()
{
	if (!cap.isOpened())  // check if we succeeded
		return -1;

	while (waitKey(100))
	{
		///normal image
		Mat frame;
		cap >> frame;
		imshow("Original Video", frame);

		///GrayScale Image
		Mat gray;
		cap >> gray;
		cvtColor(gray, gray, CV_BGR2GRAY);
		imshow("GrayScale Image", gray);

		///Edge Detedction
		Mat blur_gray;
		Mat canny_gray;
		blur(gray, blur_gray, Size(3, 3));
		Canny(blur_gray, canny_gray, 40, 40 * 2, 3);
		imshow("Edges", canny_gray);

		///Contours
		findContours(canny_gray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		Mat drawing = Mat::zeros(canny_gray.size(), CV_8UC3);
		for (int i = 0; i < contours.size(); i++)//draws all contours
		{
			///drawContours(mat drawing, vector vector point, what contours to draw, color or contors,line thickness, line type, hierarchy, maxlevel ,offset
			if (contours[i].size() >= 30)
			{
				Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));//random color for each contour
				drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());//draws the contour image
			}
		}
		imshow("Image with Contours", drawing);

		///Bonding box
		try
		{
			vector<vector<Point> > contours_poly(contours.size());
			vector<Rect> boundRect(contours.size());
			vector<Point2f>center(contours.size());
			vector<Point2f>CenterPoints(contours.size());
			vector<float>radius(contours.size());
			for (int i = 0; i < contours.size(); i++)
			{
				approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
				boundRect[i] = boundingRect(Mat(contours_poly[i]));
				minEnclosingCircle((Mat)contours_poly[i], center[i], radius[i]);
			}
			for (int i = 0; i < contours.size(); i++)
			{
				if (boundRect[i].area() > 1000)
				{
					CenterPoints[i] = Point2f((boundRect[i].width / 2) + boundRect[i].x, (boundRect[i].height / 2) + boundRect[i].y);
				}
			}
			for (int i = 0; i < contours.size(); i++)
			{
				if (boundRect[i].area() > 1000)
				{
					rectangle(frame, boundRect[i].tl(), boundRect[i].br(), RED, 2, 8, 0);
					circle(frame, Point2f((boundRect[i].width/2)+ boundRect[i].x,(boundRect[i].height/2)+ boundRect[i].y), 5, WHITE, 1, 8, 0);
				}

			}
			for (int i = 0; i < contours.size(); i++)
			{
				for (int x = 0; x < contours.size(); x++)
				{
					if (PointCompare(CenterPoints[i], CenterPoints[x]) && i != x)
					{
						circle(frame, (CenterPoints[i], CenterPoints[x]), 5, GREEN, 1, 8, 0);
					}
				}
			}

			///Rotated bounding box
			vector<RotatedRect> rotatedRect(contours.size());
			for (int i = 0; i < contours.size(); i++)
			{
				rotatedRect[i] = minAreaRect(Mat(contours[i]));//possition
			}
			for (int i = 0; i< contours.size(); i++)
			{
				Point2f rect_points[4]; rotatedRect[i].points(rect_points);

				//for (int j = 0; j < 4; j++)
					//if((Distance(rect_points[j].x, rect_points[j].y, rect_points[(j + 1) % 4].x, rect_points[(j + 1) % 4].y)) > .5)
						//line(frame, rect_points[j], rect_points[(j + 1) % 4], GREEN, 1, 8);
			}
			imshow("Bounding retangle", frame);
		}
		catch (...) {}
	}
	cout << "out of loop" << endl;
}

long double Distance( long double point1x, long double point1y, long double point2x, long double point2y)
{
	long double tempX = point2x - point1x;
	long double tempY = point2y - point1y;

	tempX = tempX*tempX;
	tempY = tempY*tempY;
	
	long double distance = sqrt(tempX - tempY);

	return(distance);
}
bool PointCompare(Point2f point1, Point2f point2)
{
	if(abs(point1.x-point2.x) < 5 && abs(point1.y-point2.y) < 5)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//640x480