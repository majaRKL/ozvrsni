/* Osnovna ideja:
 * uspoređivanje slike predoslka (znak) sa svakom slicicom i pronalazak.
 * Tamo gdje se predlozak i slicica podudaraju proglasimo znakom i 
 * nacratom pravokutnik.
 * Takav pristup daje puno lazno pozitivnih rezultata.
 * Taj problem pokusavamo rjesiti racunanjem udaljenosti izmedu 
 * trenutnog i prijasnjih slicica, te postavljamo prag ispod kojeg
 * pronadene lokacije proglasavamo znakom.
 */ 

// OpenCV zaglavlja za korištenje openCV funkcija
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace std;
using namespace cv;

// deklaracija funkcije za izracun udaljenosti izmedu dvije tocke
int calcPointDist (Point maxLoc, Point prevMaxLoc);

// vektor tocaka
vector<Point> vPoint;
Point minLoc; Point maxLoc; Point prevMaxLoc;

int main( int argc, char** argv )
{
	// kreiranje objekta cap za ucitavanje videa
    VideoCapture cap("video/znakich2.mp4");
	if(!cap.isOpened())  // check if we succeeded
        return -1;

	/* kreiranje objekata, 
	 * #rect# za definiranje regije (sredina-desno) u kojoj trazimo 
	 * znak kako ne bi pretrazivali cijelu sliku
	 * #frame# za trenutnu slicicu iz videa
	 * #roi# za slicicu regije definirane iz "rect" objekta
	 * #groi# za sivu slicicu "roi-a"
	 * #results# za spremanje rezultata pronalaska znaka 
	*/
	Rect rect;
    Mat frame, roi, groi, results;
	Mat znak1, znak2, znak3, znak4;
	
	// ucitavanje izrezanih znakova u razlicitim velicinama
	// trenutno se koristim samo znak2 
	znak1 = imread ("roi/01_roi.png");		
	znak2 = imread ("roi/02_roi.png");
	znak3 = imread ("roi/03_roi.png");
	znak4 = imread ("roi/04_roi.png");
	// prebacivanje znak2 u sliku sivih nijansi
	cvtColor (znak2, znak2, CV_BGR2GRAY);
	
	// ucitavanje slicice iz videa u frame
	// postavljanje regije interesa u roi
	cap >> frame;
	rect = Rect (600, 150, 480, 120);
    roi = frame(rect);
	
	// postavljanje velicine results ovisno o velicini roi i znak2
	int resultRows, resultCols;
	resultRows = roi.rows - znak2.rows + 1;
	resultCols = roi.cols - znak2.cols + 1;
	results.create (resultRows, resultCols, CV_32FC1);

	// pretvaranje roi u greyscale
	cvtColor(roi, groi, CV_BGR2GRAY);      
	// trazenje znaka u greyscale groi i spremanje u results
    matchTemplate (groi, znak2, results, 5);
    // normaliziranje rezultata
    normalize (results, results, 0, 1, NORM_MINMAX, -1);
    // traznje lokacije najveceg rezultata
    double minVal; double maxVal;
    minMaxLoc (results, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
    // spremanje rezultata u vektor tocaka
    vPoint.push_back (maxLoc);
    
    // Postavljanje prozora za prikaz frame-a
	// namedWindow("frame",1);
	
	// inicijalizacija varijable za udaljenost izmedu tocaka rezultata
	int dist = 0;
	
	// Ovo iznad je za prvi frame da imamo s cime usporedivat.
	// Sada ponavljaj beskonacno za sve ostale slicice
    for(;;) {        
		// ucitaj frame, postavi roi, prebaci u greyscale
        cap >> frame;              
        //imshow("frame", frame);
        roi = frame(rect);
        cvtColor(roi, groi, CV_BGR2GRAY);
        
        // pronadi podudarani template
        // normaliziraj rezultate
        // pronadi lokacije tocaka s najvecim vrijednostima
        // spremi lokacije tocaka u vektor
        matchTemplate (groi, znak2, results, 5);
        normalize (results, results, 0, 1, NORM_MINMAX, -1);
        //threshold (results, results, 0.8, 1, THRESH_BINARY);
		minMaxLoc (results, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
		vPoint.push_back (maxLoc);
		int size = vPoint.size();
		
		// Za svaki rezultat veci od 0.85 
		if (maxVal > 0.75) {
			// racunaj udaljenost izmedu dva rezultata
			// Ako je vektor tocaka manji od 10 clanova
			if (size < 10) {
				// racunaj udaljenost izmedu trenutne i prve tocke
				dist = calcPointDist (maxLoc, vPoint.at(0));
				// ako je udaljenost manja od 10 isrctaj znak
				if (dist < 10) {
					rectangle (groi, maxLoc, Point(maxLoc.x + znak2.cols, 
					maxLoc.y + znak2.rows), Scalar::all(0), 2, 18, 0 );
					//vPoint.push_back (maxLoc);	
				}
			}
			// Ako je vektor tocaka veci od 10 clanova
			else {
				// racunaj udaljenost izmedu trenutnog i sedomog prije 
				dist = calcPointDist (maxLoc, vPoint.at(size-7));  
				// iscrtaj pravokutnik ako je udaljenost manja od 15 iscrtaj pravokutnik
				if (dist < 15) {
					rectangle (groi, maxLoc, Point(maxLoc.x + znak2.cols, 
					maxLoc.y + znak2.rows), Scalar::all(0), 2, 18, 0 );
					// ponovi istu tocku u vektor kako bih imali duze pracanje znaka
					vPoint.push_back (maxLoc);	
				}
				else {
				// ponovi istu tocku u vektor kako bih imali duze pracanje znaka
				vPoint.push_back (maxLoc);
				}
			}
		}
		cout << dist << endl;
		
		// prikazi greyscale sliku regije i rezultate
        imshow("groi", groi);
        imshow("results", results);
        
        char c = cv::waitKey(15);
		/// Kontrola programa         
		// *p* zaustavi i pokreni
		// *c* pokreni ispocetka
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
