/*
 * SURFDescGenerator.cpp
 *
 *  Created on: 14-gen-2009
 *      Author: stefanocadario
 */

#include "SURFDescGenerator.h"
//#include "SURF/surflib.h"
//#include "SURF/kmeans.h"
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

	CvSeq *imageKeypoints = 0, *imageDescriptors = 0;
	IplImage* img=cvLoadImage(imagePath.c_str(), CV_LOAD_IMAGE_GRAYSCALE );

	CvSURFParams params = cvSURFParams(500, 0);
	CvMemStorage* storage = cvCreateMemStorage(0);

	cvExtractSURF(img, 0, &imageKeypoints, &imageDescriptors, storage, params );

	cout << "Object Descriptors: " << imageDescriptors->total << endl;

	list.reserve(imageKeypoints->total);
	list.resize(imageKeypoints->total);
	CvSeqReader dreader, kreader;
	cvStartReadSeq( imageKeypoints, &kreader );
	cvStartReadSeq( imageDescriptors, &dreader );

	for(int i=0; i<imageKeypoints->total; i++) {
		const CvSURFPoint* kp = (const CvSURFPoint*)kreader.ptr;
		const float* descriptor = (const float*)dreader.ptr;
		list[i] = new ImageDescriptor(64);
		list[i]->position = kp->pt;
		list[i]->setDescriptorVector((float*)descriptor, 64);
		CV_NEXT_SEQ_ELEM( kreader.seq->elem_size, kreader );
		CV_NEXT_SEQ_ELEM( dreader.seq->elem_size, dreader);
	}

	return list;
}
