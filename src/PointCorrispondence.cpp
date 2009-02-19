/*
 * PointCorrispondence.cpp
 *
 *  Created on: 14-gen-2009
 *      Author: stefanocadario
 */

#include "PointCorrispondence.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxtypes.h>

using namespace std;

PointCorrispondence::PointCorrispondence() {
	// TODO Auto-generated constructor stub

}

PointCorrispondence::PointCorrispondence(float x1, float y1, float x2, float y2, double diff) {
	p1.x = x1;
	p1.y = y1;
	p2.x = x2;
	p2.y = y2;
	difference = diff;
}

void PointCorrispondence::setPoint1(float x, float y) {
	p1.x = x;
	p1.y = y;
}

void PointCorrispondence::setPoint2(float x, float y) {
	p2.x = x;
	p2.y = y;
}

void PointCorrispondence::setPoints(CvPoint2D32f punto1, CvPoint2D32f punto2) {
	p1 = punto1;
	p2 = punto2;
}

void PointCorrispondence::setPoints(float x1, float y1, float x2, float y2){
	p1.x = x1;
	p1.y = y1;
	p2.x = x2;
	p2.y = y2;
}

void PointCorrispondence::printPoints() {
	cout << "[" << p1.x << "," << p1.y << "] <--> [" << p2.x << "," << p2.y << "]";
}

string PointCorrispondence::getScript(){
	string script = "c";
	stringstream n1, n2, x1, x2, y1, y2;

	//qui si dovrebbero aggiungere gli indici della relativa immagine, ma dato che al momento funziona con solo due immagini...
	n1 << 0;
	n2 << 1;
	x1 << p1.x;
	x2 << p2.x;
	y1 << p1.y;
	y2 << p2.y;

	script += " n" + n1.str() + " N" + n2.str() + " x" + x1.str() + " y" + y1.str() + " X" + x2.str() + " Y" + y2.str();
	return script;
}

double PointCorrispondence::computeError(CvMat* fundamental_matrix) {
	// For 2 points related with Foundamental matrix should be: a*F*b = 0
	CvMat *pm1 = cvCreateMat(3,1,CV_32FC1);
	cvmSet(pm1, 0, 0, p1.x);
	cvmSet(pm1, 1, 0, p1.y);
	cvmSet(pm1, 2, 0, 1.0f);
	CvMat *pm2 = cvCreateMat(3,1,CV_32FC1);
	cvmSet(pm2, 0, 0, p2.x);
	cvmSet(pm2, 1, 0, p2.y);
	cvmSet(pm2, 2, 0, 1.0f);
	CvMat *dst = cvCreateMat(1,3,CV_32FC1);
	CvMat *final = cvCreateMat(1,1,CV_32FC1);
	cvGEMM(pm2, fundamental_matrix, 1, NULL, 1, dst, CV_GEMM_A_T );
	cvGEMM(dst, pm1, 1, NULL, 1, final,0 );

	return pow(cvmGet(final, 0, 0),2);
}


// TODO: this function is taken from openCv: "cvFundm.cpp" in "CvFMEstimator::computeReprojError"
// DUNNO if is correct...
/*
double PointCorrispondence::computeError(CvMat* fundamental_matrix) {
	const float* F = fundamental_matrix->data.fl;
	float err;
	double a, b, c, d1, d2, s1, s2;

	a = F[0]*p1.x + F[1]*p1.y + F[2];
	b = F[3]*p1.x + F[4]*p1.y + F[5];
	c = F[6]*p1.x + F[7]*p1.y + F[8];

	s2 = 1./(a*a + b*b);
	d2 = p2.x*a + p2.y*b + c;

	a = F[0]*p2.x + F[3]*p2.y + F[6];
	b = F[1]*p2.x + F[4]*p2.y + F[7];
	c = F[2]*p2.x + F[5]*p2.y + F[8];

	s1 = 1./(a*a + b*b);
	d1 = p1.x*a + p1.y*b + c;

	err = (float)(d1*d1*s1 + d2*d2*s2);
	return 1/err;
}
*/

PointCorrispondence::~PointCorrispondence() {
	// TODO Auto-generated destructor stub
}
