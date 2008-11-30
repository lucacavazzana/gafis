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

float ImageDescriptor::compare(ImageDescriptor *x) {
	float difference = 0;
	vector<int> xDescriptors = x->getDescriptorVector();	// TODO: Using = mean that all data will be copied
															// it's not very good...
	//cout << "Size: " << xDescriptors.size() << endl;
	for(int i=0; i<descriptorVector.size(); i++) {
		if(descriptorVector[i]!=xDescriptors[i]) return 1;
		/*
		difference += descriptorVector[i]-xDescriptors[i];
		*/
	}
	return 0;
}
