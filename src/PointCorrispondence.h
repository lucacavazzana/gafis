/*
 * PointCorrispondence.h
 *
 *  Created on: 14-gen-2009
 *      Author: stefanocadario
 */

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxtypes.h>
#include <iostream>

#ifndef POINTCORRISPONDENCE_H_
#define POINTCORRISPONDENCE_H_

class PointCorrispondence {
public:
	PointCorrispondence();
	PointCorrispondence(float x1, float y1, float x2, float y2, double diff);
	virtual ~PointCorrispondence();
	void setPoint1(float x, float y);
	void setPoint2(float x, float y);
	void setPoints(CvPoint2D32f punto1, CvPoint2D32f punto2);
	void setDifference(double value) {
		difference = value;
	}
	double getDifference() {
		return difference;
	}
	CvPoint2D32f p1;
	CvPoint2D32f p2;
	// TODO: this class can contain "ImageDescriptor"...

	void printPoints();
	double computeError(CvMat* fundamental_matrix);
protected:
	double difference;
};

#endif /* POINTCORRISPONDENCE_H_ */
