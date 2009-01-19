/*
 * SURFDescriptor.h
 *
 *  Created on: 15-gen-2009
 *      Author: stefanocadario
 */

#ifndef SURFDESCRIPTOR_H_
#define SURFDESCRIPTOR_H_

#include "ImageDescriptor.h"

class SURFDescriptor: public ImageDescriptor {
public:
	SURFDescriptor();
	virtual ~SURFDescriptor();
	virtual float compare(SURFDescriptor & x);
	int getLaplacian() {
		return laplacian;
	}
	void setLaplacian(int value) {
		laplacian = value;
	}

private:
	int laplacian;
};

#endif /* SURFDESCRIPTOR_H_ */
