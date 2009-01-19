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

#include "SURF/surflib.h"



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
#define NTOFIND 2
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

#define MAXDOUBLE 10000
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

/*
	// COMPUTING FOUNDAMENTAL MATRIX
	CvMat* points1;
	CvMat* points2;
	CvMat* status;
	CvMat* fundamental_matrix;
	points1 = cvCreateMat(1,8,CV_32FC2);
	points2 = cvCreateMat(1,8,CV_32FC2);
	status = cvCreateMat(1,8,CV_8UC1);
	for( int i = 0; i < 8; i++ )
	{
	    points1->data.fl[i*2]   = corrispondence[i].p1.x;
	    points1->data.fl[i*2+1] = corrispondence[i].p1.y;
	    points2->data.fl[i*2]   = corrispondence[i].p2.x;
	    points2->data.fl[i*2+1] = corrispondence[i].p2.y;
	}
	fundamental_matrix = cvCreateMat(3,3,CV_32FC1);
	int fm_count = cvFindFundamentalMat( points1, points2,
	                                     fundamental_matrix,
	                                     CV_FM_8POINT,1.0,0.99,status);

	cout << "fm_count: " << fm_count << endl;

	cout << "Error calculated: " << corrispondence[0].computeError(fundamental_matrix) << endl;
	cout << "Error calculated: " << corrispondence[1].computeError(fundamental_matrix) << endl;
	cout << "Error calculated: " << corrispondence[2].computeError(fundamental_matrix) << endl;
	cout << "Error calculated: " << corrispondence[25].computeError(fundamental_matrix) << endl;

	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			cout << CV_MAT_ELEM( *fundamental_matrix, float, i, j) << "\t";
		}
		cout << endl;
	}
	*/


	logger->Log("That's all folks", 2);
	return 0;
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
