/*
 * SURFDescGenerator.cpp
 *
 *  Created on: 14-gen-2009
 *      Author: stefanocadario
 */

#include "SURFDescGenerator.h"
#include "SURF/surflib.h"
#include "SURF/kmeans.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace std;

SURFDescGenerator::SURFDescGenerator() {
	// TODO Auto-generated constructor stub

}

SURFDescGenerator::~SURFDescGenerator() {
	// TODO Auto-generated
}

vector<ImageDescriptor*> SURFDescGenerator::generateDescriptors() {

	vector<ImageDescriptor*> list;

	// Declare Ipoints and other stuff
	IpVec ipts;
	IplImage *img=cvLoadImage(imagePath.c_str());

	// Detect and describe interest points in the image
	//surfDetDes(img, ipts, false, 3, 4, 1, 0.0004f);
	surfDetDes(img, ipts, false, 3, 4, 1, 0.001f);

	list.resize(ipts.size());

	for(int i=0; i<ipts.size(); i++) {
	  list[i] = new ImageDescriptor(64);
	  list[i]->position.x = ipts[i].x;
	  list[i]->position.y = ipts[i].y;
	  list[i]->setDescriptorVector(ipts[i].descriptor, 64);
	}

	// Draw the detected points
	// drawIpoints(img, asdlol);

	// Display the result
	// showImage(img);
	return list;
}
