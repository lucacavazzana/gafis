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
#include <HashTable.h>


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

	generator1 = new ColorDescGenerator();
	generator1->setImagePath(images[0]);
	vector<ImageDescriptor*> vec1= generator1->generateDescriptors();
	generator2 = new ColorDescGenerator();
	generator2->setImagePath(images[1]);
	vector<ImageDescriptor*> vec2= generator2->generateDescriptors();

	// TODO: rewrite with pointer (reference) and use btree/hash function

	/*

	// COMPARE ONE BY ONE FUNCTION

	int common_descriptors =0;
	for(int i=3500; i<vec1.size(); i++) {
		cout << (float)i*100/vec1.size() << "% " << endl;
		for(int j=0; j<vec2.size(); j++) {
			if(!vec1[i]->compare(vec2[j])) {
				cout << "FOUNDED..." << common_descriptors++ << endl;
			}
		}
	}

	cout << "Number common descriptor = " << common_descriptors << endl;

	*/

	// COMPARE WITH HASHING FUNCTION

	HashTable htable = HashTable(vec1.size());

	int collision = 0; 	// number of collisions founded (for statistics purpose)
	int emptyCell = 0;	// number of empty cell of array

	for(int i=0; i<vec1.size(); i++)	// Hashing first vector
	{
		htable.addElement(vec1[i]->getHash(), i);
	}

	hashElement *temp;
	int foundedElements = 0;
	int confronti=0;

	int searchTollerance = 60;

	for(int j=0; j<vec2.size(); j++)
	{
		for(int k=-searchTollerance;k<searchTollerance;k++) {
			temp = htable.getElement(vec2[j]->getHash()+k);
			//cout << "temp: " << temp;
			while(temp->index!=-1) {
				//cout << "temp->index: " << temp->index << endl;
				if((vec1[temp->index]->compare(vec2[j]))<=searchTollerance) {				//cout << "found " << endl;
					foundedElements++;
					cout << "Founded: [" << vec1[temp->index]->x << ";"<< vec1[temp->index]->y <<"] and [" << vec2[j]->x << ";"<< vec2[j]->y << "]" << endl;
				}
				confronti++;
				temp = temp->nextRecord;
			}
		}
		//cout << "--------" << endl;
	}

	cout << "Founded elements: " << foundedElements << endl;
	cout << "Confronti: " << confronti << endl;

	//htable.elaborateStats();

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
