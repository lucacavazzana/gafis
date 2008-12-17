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

using namespace std;

class ImageDescriptor {
public:
	ImageDescriptor();
	ImageDescriptor(int dimensionality);
	virtual ~ImageDescriptor();
	void setDescriptorVector(vector<int> inVector);
	vector<int> getDescriptorVector();
	float compare(ImageDescriptor *x);
	int getHash();
	void printDescVector();
	int x;
	int y;
	double scale;
	double orientation;
	double cornerness;
protected:
	vector<int> descriptorVector;
};

#endif /* IMAGEDESCRIPTOR_H_ */
