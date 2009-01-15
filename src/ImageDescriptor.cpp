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
	sum = -1;
	hashNormalizer = 1;
}

ImageDescriptor::ImageDescriptor(int dimensionality) {
	sum = -1;
	descriptorVector.reserve(dimensionality);
	hashNormalizer = 1;
}

ImageDescriptor::~ImageDescriptor() {

}

void ImageDescriptor::setDescriptorVector(float array[], int size) {
	//if(descriptorVector.size()>0) {
	//	descriptorVector.clear();
	//}
	descriptorVector.clear();
	descriptorVector.resize(size);
	for(int i=0; i<size; i++) {
		descriptorVector[i] = array[i];
	}
}

void ImageDescriptor::setDescriptorVector(vector<float> inVector) {
	//if(descriptorVector.size()>0) {
	//	descriptorVector.clear();
	//}
	descriptorVector.clear();
	descriptorVector = inVector; // TODO: just copy instance or i must copy all data ?
}

vector<float> ImageDescriptor::getDescriptorVector() {
	return descriptorVector;
}

float ImageDescriptor::getHash() {
	if(sum==-1) {
		sum = 0;
		for(int i=0; i<descriptorVector.size(); i++) {
				if(descriptorVector[i]<0)
					sum -= (descriptorVector[i]);
				else
					sum += (descriptorVector[i]);
		}
	}
	return sum*hashNormalizer;
}

void ImageDescriptor::printDescVector() {
	for(int i=0; i<descriptorVector.size(); i++) {
		if((i+1)%2==0 || (i+1)%4==0)
			cout << descriptorVector[i] << endl;
	}
}

void ImageDescriptor::setNormalizer(float value) {
	hashNormalizer = value;
}

float ImageDescriptor::getNormalizer() {
	return hashNormalizer;
}

/*
 * REFACTORED ! Little improvement
float ImageDescriptor::compare(ImageDescriptor *x) {
	float difference = 0;
	vector<int> xDescriptors = x->getDescriptorVector();	// TODO: Using = mean that all data will be copied
															// it's not very good...
	//cout << "Size: " << xDescriptors.size() << endl;
	for(int i=0; i<descriptorVector.size(); i++) {
		//if(descriptorVector[i]!=xDescriptors[i]) return 1;

		difference += abs(descriptorVector[i]-xDescriptors[i]);

	}
	return difference;
	//return 0;
}
*/

float ImageDescriptor::compare(ImageDescriptor & x) {
	float difference = 0;
	float singleDiff;
	for(int i=0; i<descriptorVector.size(); i++) {
		singleDiff = pow(descriptorVector[i] - x.getDescriptorVector()[i],2);
		difference += singleDiff;
	}
	difference = pow(difference, 0.5);
	return difference;
}
