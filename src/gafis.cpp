//============================================================================
// Name        : gafis.cpp
// Author      : Cadario Stefano & Cavazzana Luca
// Version     :
// Copyright   : Your copyright notice
//============================================================================

#include <iostream>
#include <sstream>

#include <Logger.h>
#include <ImageDescriptor.h>
#include <DescGenerator.h>
#include <ColorDescGenerator.h>
#include <SURFDescGenerator.h>
#include <HashTable.h>
#include <PointCorrispondence.h>
#include <GASAC.h>

#include <ga/ga.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxtypes.h>

//#include "SURF/surflib.h"



/* DEBUG_LVL = Livello di debug
 * definisce il livello di "verbose" dell'applicazione
 * 0 = Nessun messaggio in output di debug
 * 1 = Solo messaggi di errore in output
 * 2 = Messaggi informativi e di errore in output
 * 3 = Messaggi informativi, di errore e warning
 * 4 = Massimo livello possibile di debug */
#define DEBUG_LVL 3

// MAX_IMAGES = Numero massimo di immagini che  possibile elaborare
#define MAX_IMAGES 2

// BEST-BIN-FIRST SEARCH PARAMETERS
#define NTOFIND 5
#define MAXDEEPTH 20

#define NCOUPLES 100

//#define cout STD_COUT

using namespace std;

// Global Variables
Logger *logger;
string images[MAX_IMAGES];
int nimages=0;
DescGenerator *generator1;
DescGenerator *generator2;

// Functions declarations
int parseArguments(int argc, char **argv);
void printHelp();
void viewBothImages(IplImage *img1, IplImage *img2,PointCorrispondence *corrispondence, int *best, int size);


int main(int argc, char **argv) {

	logger = Logger::getInstance();
	logger->setLevel(DEBUG_LVL);		// TODO: this parameter must be passed with command line args


	// Parse all arguments and set settings
	if(parseArguments(argc,argv)) {
		printHelp(); // if there's too few arguments print help and exit
		return 0;
	}

	//generator1 = new ColorDescGenerator();
	generator1 = new SURFDescGenerator();
	generator1->setImagePath(images[0]);
	vector<ImageDescriptor*> vec1;
	vec1 = generator1->generateDescriptors();
	logger->Log("Descriptors of first image complete", 2);


	generator2 = new SURFDescGenerator();
	generator2->setImagePath(images[1]);
	vector<ImageDescriptor*> vec2= generator2->generateDescriptors();
	logger->Log("Descriptors of second image complete", 2);

	//vec2 = vec1; // PROVA DEL 9 :D should return all values = 0;

	// Using best-bin-first algo (included in OpenCV)

	logger->Log("Creating matrix of feature (first image)", 2);

	CvMat* firstDesc = cvCreateMat(vec1.size(),64,CV_32FC1);

	for(int i=0; i<vec1.size(); i++) {
		for(int j=0; j<64; j++) {
			cvmSet(firstDesc, i, j, vec1[i]->getDescriptorVector()[j]);
		}
	}

	logger->Log("Matrix of feature complete", 2);

	CvFeatureTree* firstImageTree;
	firstImageTree = cvCreateFeatureTree(firstDesc);

	logger->Log("Feature Tree complete", 2);

	logger->Log("Creating matrix of feature (second image)", 2);

	CvMat* secondDesc = cvCreateMat(vec2.size(),64,CV_32FC1);

	for(int i=0; i<vec2.size(); i++) {
		for(int j=0; j<64; j++) {
			cvmSet(secondDesc, i, j, vec2[i]->getDescriptorVector()[j]);
		}
	}

	CvMat *results = cvCreateMat(vec2.size(), NTOFIND, CV_32SC1);
	CvMat *distances = cvCreateMat(vec2.size(), NTOFIND, CV_64FC1);

	logger->Log("Finding similar points...", 2);

	cvFindFeatures(firstImageTree, secondDesc, results, distances, NTOFIND, MAXDEEPTH);

	logger->Log("Points search complete", 2);

	// Selecting best 'NCOUPLES' couples
	PointCorrispondence corrispondence[NCOUPLES];
	double maxValue = 0;
	double tempMax = 0;
	int founded = 0;
	double tempMin;

#define MAXDOUBLE 100000
	for(int i=0; i<NCOUPLES; i++) {
		corrispondence[i].setDifference(MAXDOUBLE);
	}
	maxValue = MAXDOUBLE;

	//cout << "Max Value: " << maxValue;
	// Search for best couples (min distance)
	for(int i=0; i<vec2.size(); i++) {
		// Find min distance of same image2's point
		founded = 0;
		tempMin = cvmGet(distances, i, 0);
		for(int j=1; j<NTOFIND; j++) {
				if(tempMin>cvmGet(distances, i, j)) {
						founded = j;
						tempMin = cvmGet(distances,i,j);
				}
		}
		// If the point has distance below (is more similary) with the actual max of corrispondance
		if(cvmGet(distances, i, founded)<maxValue) {
			// Search the element to be substituted in candidate best array (corrispondence)
			// and meanwhile search for the new maxValue
			tempMax = 0;
			bool substituted = false;
			for(int j=0; j<NCOUPLES; j++) {
				// Seach for element to substitute
				if(corrispondence[j].getDifference()==maxValue && !substituted) {
					// FOUND, replace it with new value
					substituted = true;
					corrispondence[j].setDifference(cvmGet(distances, i, founded));
					corrispondence[j].setPoints(vec1[cvGet2D(results,i,founded).val[0]]->position,vec2[i]->position);
				}
				// Search for new maxValue
				if(corrispondence[j].getDifference()>tempMax) {
					tempMax = corrispondence[j].getDifference();
				}
			}
			maxValue = tempMax;
		}
	}

	for(int i=0; i<NCOUPLES; i++) {
		cout << "Difference: " << corrispondence[i].getDifference() << endl;
		corrispondence[i].printPoints();
	}

	cvReleaseFeatureTree(firstImageTree);

	GASAC *ga = GASAC::getInstance();
	ga->setPoints(corrispondence, NCOUPLES);
	ga->run();

	IplImage *img1=cvLoadImage(images[0].c_str());
	IplImage *img2=cvLoadImage(images[1].c_str());
	int *bestCouples = ga->bestCouples;
	int geneSize = ga->geneSize;
	viewBothImages(img1,img2,corrispondence,bestCouples,geneSize);


	logger->Log("That's all folks", 2);
	return 0;
}

void viewBothImages(IplImage *img1, IplImage *img2,PointCorrispondence *corrispondence, int *best, int size) {

	CvSize dim;
	dim.height = img1->height;
	dim.width = img1->width+img2->width;
	IplImage* final = cvCreateImage(dim, img1->depth, img1->nChannels);
	//cout << "Channels: " << img1->nChannels << " " << img2->nChannels << endl;

	int nl= img1->height; // number of lines
	int nc1= img1->width * img1->nChannels; // total number of element per line
	int step1= img1->widthStep; // effective width
	int step2= img2->widthStep;
	int finalstep = final->widthStep;

	      // get the pointer to the image buffer
	unsigned char *finaldata= reinterpret_cast<unsigned char *>(final->imageData);

	unsigned char *data1= reinterpret_cast<unsigned char *>(img1->imageData);

	unsigned char *data2= reinterpret_cast<unsigned char *>(img2->imageData);

	for (int i=1; i<nl; i++) {
		for (int j=0; j<nc1; j+= img1->nChannels) {
			finaldata[j]= data1[j];
			finaldata[j+1]= data1[j+1];
			finaldata[j+2]= data1[j+2];
		}
		data1+=step1;
		finaldata+=finalstep;
	}
	finaldata = reinterpret_cast<unsigned char *>(final->imageData);
	nl= img1->height; // number of lines
	int nc2= img1->width * img1->nChannels; // total number of element per line

	for (int i=1; i<nl; i++) {
		for (int j=0; j<nc2; j+= img2->nChannels) {

		// process each pixel ---------------------
		finaldata[j+nc1]= data2[j];
		finaldata[j+1+nc1]= data2[j+1];
		finaldata[j+2+nc1]= data2[j+2];

		// end of pixel processing ----------------

		} // end of line
		data2+=step2;
		finaldata+=finalstep;
	}

	for(int i=0; i<size; i++) {
		CvPoint p2 = cvPointFrom32f(corrispondence[best[i]].p2);
		p2.x+=img1->width;
		cvLine(final,cvPointFrom32f(corrispondence[best[i]].p1),p2,cvScalar(0, 255, 0));
	}
	cvNamedWindow("gafis", CV_WINDOW_AUTOSIZE );
	cvShowImage("gafis", final);
	cvWaitKey(0);
}

void printHelp() {
	logger->printMessage("Gafis - Cadario Stefano & Cavazzana Luca");
	logger->printMessage("use: gafis firstimage.jpg secondimage.jpg");
	logger->printMessage("options:");
	logger->printMessage("No OPTION FOR NOW...");
}

int parseArguments(int argc, char **argv) {

	// TODO: better arguments parser, we can use a library for that !
	for(int i=1; i<argc; i++) {
		ostringstream argomenti(ostringstream::out); // TODO: it can stay out of for but doesn't clear correctly
		argomenti << "arg " << i << ": " << argv[i];
		logger->Log(argomenti.str(), 3);
		argomenti.clear();
		if(argv[i][0]=='-') { // It's an option argument
			// Code options here
		}
		else { // Must be a file name
			if(nimages<MAX_IMAGES) {
				images[nimages++] = string(argv[i]);
			}
		}
	}
	// Se non ho sufficienti argomenti (almeno 2 immagini) ritorno 1 (errore)
	if(nimages<2) {
		return 1;
	}
	else return 0;
}
