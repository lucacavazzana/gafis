/*
 * ImageDescriptor.cpp
 *
 *  Created on: 24-nov-2008
 *      Author: stefanocadario
 */

#include "ImageDescriptor.h"
#include <vector>

using namespace std;

ImageDescriptor::ImageDescriptor() {

}

ImageDescriptor::ImageDescriptor(int dimensionality) {
	descriptorVector.reserve(dimensionality);
}

ImageDescriptor::~ImageDescriptor() {

}

void ImageDescriptor::setDescriptorVector(vector<int> inVector) {
	//if(descriptorVector.size()>0) {
	//	descriptorVector.clear();
	//}
	descriptorVector.clear();
	descriptorVector = inVector; // TODO: just copy instance or i must copy all data ?
}

vector<int> ImageDescriptor::getDescriptorVector() {
	return descriptorVector;
}

int ImageDescriptor::getHash() {
	int sum = 1;
	for(int i=0; i<descriptorVector.size(); i++) {
			sum += (descriptorVector[i]);
	}
	return sum;
}

void ImageDescriptor::printDescVector() {
	for(int i=0; i<descriptorVector.size(); i++) {
				cout << descriptorVector[i] << endl;
	}
}

float ImageDescriptor::compare(ImageDescriptor *x) {
	float difference = 0;
	vector<int> xDescriptors = x->getDescriptorVector();	// TODO: Using = mean that all data will be copied
															// it's not very good...
	//cout << "Size: " << xDescriptors.size() << endl;
	for(int i=0; i<descriptorVector.size(); i++) {
		//if(descriptorVector[i]!=xDescriptors[i]) return 1;

		difference += abs(descriptorVector[i]-xDescriptors[i]);

	}
	return difference/descriptorVector.size();
	//return 0;
}
