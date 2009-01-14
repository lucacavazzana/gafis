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

PointCorrispondence::PointCorrispondence() {
	// TODO Auto-generated constructor stub

}

PointCorrispondence::PointCorrispondence(int x1, int y1, int x2, int y2) {
	p1.x = x1;
	p1.y = y1;
	p2.x = x2;
	p2.y = y2;
}

void PointCorrispondence::setPoint1(int x, int y) {
	p1.x = x;
	p1.y = y;
}

void PointCorrispondence::setPoint2(int x, int y) {
	p2.x = x;
	p2.y = y;
}

void PointCorrispondence::setPoints(CvPoint punto1, CvPoint punto2) {
	p1 = punto1;
	p2 = punto2;
}

PointCorrispondence::~PointCorrispondence() {
	// TODO Auto-generated destructor stub
}
