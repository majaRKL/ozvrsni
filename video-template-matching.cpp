#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int main( int argc, char** argv )
{
    VideoCapture cap("znakich2.mp4");
	if(!cap.isOpened())  // check if we succeeded
        return -1;

	Rect rect;
    Mat roi;
    //namedWindow("frame",1);
    namedWindow("roi",1);
    for(;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera              
        //imshow("frame", frame);
        rect = Rect (600, 150, 480, 120);
        roi = frame(rect);
        imshow("roi", roi);
        if(waitKey(30) >= 0) break;
    }
    return 0;
}
