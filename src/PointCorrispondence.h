/*
 * PointCorrispondence.h
 *
 *  Created on: 14-gen-2009
 *      Author: stefanocadario
 */

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxtypes.h>

#ifndef POINTCORRISPONDENCE_H_
#define POINTCORRISPONDENCE_H_

class PointCorrispondence {
public:
	PointCorrispondence();
	PointCorrispondence(int x1, int y1, int x2, int y2);
	virtual ~PointCorrispondence();
	void setPoint1(int x, int y);
	void setPoint2(int x, int y);
	void setPoints(CvPoint punto1, CvPoint punto2);
	CvPoint p1;
	CvPoint p2;
};

#endif /* POINTCORRISPONDENCE_H_ */
