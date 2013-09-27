#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int main( int argc, char** argv )
{
    VideoCapture cap("video/znakich2.mp4");
	if(!cap.isOpened())  // check if we succeeded
        return -1;

	Rect rect;
    Mat frame, roi, groi, results;
	Mat znak1, znak2, znak3, znak4;
	znak1 = imread ("roi/01_roi.png");		
	znak2 = imread ("roi/02_roi.png");
	znak3 = imread ("roi/03_roi.png");
	znak4 = imread ("roi/04_roi.png");
	cvtColor (znak2, znak2, CV_BGR2GRAY);
	
	cap >> frame;
	rect = Rect (600, 150, 480, 120);
    roi = frame(rect);
	
	int resultRows, resultCols;
	resultRows = roi.rows - znak1.rows + 1;
	resultCols = roi.cols - znak1.cols + 1;
	results.create (resultRows, resultCols, CV_32FC1);
	
	//namedWindow("frame",1);
	
	double minVal; double maxVal; 
	Point minLoc; Point maxLoc; Point matchLoc;
    int n = 0;
    for(;;)
    {        
        cap >> frame;              
        //imshow("frame", frame);
        roi = frame(rect);
        cvtColor(roi, groi, CV_BGR2GRAY);
        
        matchTemplate (groi, znak2, results, 5);
        normalize (results, results, 0, 1, NORM_MINMAX, -1);
        //threshold (results, results, 0.8, 1, THRESH_BINARY);
		minMaxLoc (results, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
		if (maxVal > 0.85) {
			n++;
			if (n > 3) {
			rectangle (groi, maxLoc, Point( maxLoc.x + znak2.cols, 
					maxLoc.y + znak2.rows), Scalar::all(0), 2, 18, 0 );
			n = 0;
			}
		}
        imshow("groi", groi);
        imshow("results", results);
        
        if(waitKey(30) >= 0) break;
    }
    return 0;
}
