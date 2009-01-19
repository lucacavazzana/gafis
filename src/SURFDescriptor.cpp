/*
 * SURFDescriptor.cpp
 *
 *  Created on: 15-gen-2009
 *      Author: stefanocadario
 */

#include "SURFDescriptor.h"

SURFDescriptor::SURFDescriptor() : ImageDescriptor(64) {
	// TODO Auto-generated constructor stub
}

SURFDescriptor::~SURFDescriptor() {
	// TODO Auto-generated destructor stub
}

float SURFDescriptor::compare(SURFDescriptor & x) {
	//SURFDescriptor *asd = (SURFDescriptor)x;
	if(laplacian!=x.getLaplacian()) {
		float difference = 0;
		float singleDiff;
		for(int i=0; i<descriptorVector.size(); i++) {
			singleDiff = pow(descriptorVector[i] - x.getDescriptorVector()[i],2);
			difference += singleDiff;
		}
		difference = pow(difference, 0.5);
		return difference;
	}
	return 64.0f; // TODO: what is maximun difference ?
}
