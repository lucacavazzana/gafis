/*
 * ImageDescriptor.h
 *
 *  Created on: 24-nov-2008
 *      Author: stefanocadario
 */

#ifndef IMAGEDESCRIPTOR_H_
#define IMAGEDESCRIPTOR_H_

#include <iostream>
#include <vector>
#include <opencv/cxtypes.h>

using namespace std;

class ImageDescriptor {
public:
	ImageDescriptor();
	ImageDescriptor(int dimensionality);
	virtual ~ImageDescriptor();
	void setDescriptorVector(vector<float> inVector);
	void ImageDescriptor::setDescriptorVector(float array[], int size);
	vector<float> getDescriptorVector();
	virtual float compare(ImageDescriptor & x);
	float getHash();
	void setNormalizer(float value);
	float getNormalizer();
	void printDescVector();
	CvPoint position;
	//int x;
	//int y;
	double scale;
	double orientation;
	double cornerness;
protected:
	vector<float> descriptorVector;
	float sum;
	float hashNormalizer;
};

#endif /* IMAGEDESCRIPTOR_H_ */
