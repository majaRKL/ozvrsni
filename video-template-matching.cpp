#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace std;
using namespace cv;

int calcPointDist (Point maxLoc, Point prevMaxLoc);

vector<Point> vPoint;
Point minLoc; Point maxLoc; Point prevMaxLoc;

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

	double minVal; double maxVal; 
	cvtColor(roi, groi, CV_BGR2GRAY);        
    matchTemplate (groi, znak2, results, 5);
    normalize (results, results, 0, 1, NORM_MINMAX, -1);
    minMaxLoc (results, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
    vPoint.push_back (maxLoc);
	//namedWindow("frame",1);
	int dist = 0;
    for(;;) {        
        cap >> frame;              
        //imshow("frame", frame);
        roi = frame(rect);
        cvtColor(roi, groi, CV_BGR2GRAY);
        
        matchTemplate (groi, znak2, results, 5);
        normalize (results, results, 0, 1, NORM_MINMAX, -1);
        //threshold (results, results, 0.8, 1, THRESH_BINARY);
		minMaxLoc (results, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
		vPoint.push_back (maxLoc);
		int size = vPoint.size();
		
		if (maxVal > 0.85) {
			if (size < 10) {
				dist = calcPointDist (maxLoc, vPoint.at(0));  
				if (dist > 1) {
					vPoint.push_back (maxLoc);	
				}
				else {
				rectangle (groi, maxLoc, Point(maxLoc.x + znak2.cols, 
					maxLoc.y + znak2.rows), Scalar::all(0), 2, 18, 0 );
				vPoint.push_back (maxLoc);
				}
			}
			else {
				dist = calcPointDist (maxLoc, vPoint.at(size-8));  
				if (dist > 5) {
					vPoint.push_back (maxLoc);	
				}
				else {
				rectangle (groi, maxLoc, Point(maxLoc.x + znak2.cols, 
					maxLoc.y + znak2.rows), Scalar::all(0), 2, 18, 0 );
				vPoint.push_back (maxLoc);
				}
			}
		}
		cout << dist << endl;
		/*
		 * Nova ideja:
		 * Usporedivat proslu i trenutnu maxLoc ako je razlika vec od
		 * npr 20 pixela na crtati rectangle 
		 */
        imshow("groi", groi);
        imshow("results", results);
        
        char c = cv::waitKey(15);
		/// Kontrola programa         
        switch( c ) {
			case 'c': 
				cap.set(CV_CAP_PROP_POS_FRAMES,0); 
				break;    
			case 'p': 
				pause:
                char d; d = cv::waitKey(0);
                if (d =='p') break;
                else goto pause;
            case 27:
				return 0;
		}        
    }
    return 0;
}

int calcPointDist (Point maxLoc, Point prevMaxLoc)
{
	int dist = 0;
	dist = sqrt ((maxLoc.x - prevMaxLoc.x) * (maxLoc.x - prevMaxLoc.x) +
				 (maxLoc.y - prevMaxLoc.y) * (maxLoc.y - prevMaxLoc.y));	
	cout << dist << endl;
	return dist;
}

