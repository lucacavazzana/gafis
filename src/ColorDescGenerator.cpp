/*
 * ColorDescGenerator.cpp
 *
 *  Created on: 24-nov-2008
 *  Author: stefanocadario
 */

#include "ColorDescGenerator.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

ColorDescGenerator::ColorDescGenerator():DescGenerator() {
	bin_path="../../colorDescriptor";
}

ColorDescGenerator::~ColorDescGenerator() {

}

vector<ImageDescriptor*> ColorDescGenerator::generateDescriptors() {
	// std::cout << "asd: " << imagePath << endl;
	string cmdline;
	cmdline = bin_path + " " + imagePath + " --output lol.txt";
	//system(cmdline.c_str());
	return textFileParser(imagePath+".txt");
}

vector<ImageDescriptor*> ColorDescGenerator::textFileParser(string filepath) {

	ifstream in(filepath.c_str());
	string read;
	int DescriptorsDimensionality = 0;
	int Ndescriptors = 0;
	istringstream inStream;
	string temp;
	vector<int> tempArray; 				// Temporary array allocated single time, contains descriptor vector

	// TODO: prevedere la possibilitˆ di riempire un albero binario
	vector<ImageDescriptor*> list;

	int i = 0; // used as index for the list of descriptor

	// Read Marker (FIRST LINE)
	getline(in, read);
	logInstance->Log("First line: " + read, 4);
	if(read != "KOEN1") {
		logInstance->Log("ERROR - KOEN1 not found in first line of file", 1);
		// return NULL;
	}

	// Read Descriptor Dimensionality (SECOND LINE)
	getline(in, read);
	inStream.str(read);
	logInstance->Log("Second line (Descriptors Dimensionality): " + read, 4);
	inStream >> DescriptorsDimensionality;
	inStream.clear();

	// Read Number of Descriptors (THIRD LINE)
	getline(in, read);
	inStream.str(read);
	logInstance->Log("Third line (Number of descriptors): " + read, 4);
	inStream >> Ndescriptors;
	inStream.clear();

	if(Ndescriptors==0 || DescriptorsDimensionality==0) {
		// TODO: something wrong, it couldn't be! Launch exception ?
		return list;
	}

	list.resize(Ndescriptors);

	//cout << "asd1: " << DescriptorsDimensionality << endl;
	//cout << "asd2: " << Ndescriptors;

	tempArray.resize(DescriptorsDimensionality);

	logInstance->Log("Reading descriptors...", 3);

	while(getline(in, read) && i<Ndescriptors) {
		inStream.str(read);
		logInstance->Log("Descriptor read: " + read, 4);
		list[i] = new ImageDescriptor();
		inStream >> temp;
		if(temp.compare("<CIRCLE")) {
			// TODO: exception
			logInstance->Log("Error expected '<CIRCLE': found '" + temp + "'", 2);
		}
		inStream >> list[i]->x;
		inStream >> list[i]->y;
		inStream >> list[i]->scale;
		inStream >> list[i]->orientation;
		inStream >> list[i]->cornerness;
		inStream >> temp;
		if(temp.compare(">;")) {
			// TODO: exception
			logInstance->Log("Error expected '>;': found '" + temp + "'", 2);
		}

		for(int m=0; m<DescriptorsDimensionality; m++) {
			inStream >> tempArray[m];				// I write the data in a temporary array
		}
		list[i]->setDescriptorVector(tempArray);	// Temporary array it's copied on Object new array
													// There's no reference so i can use the same array
													// With pointers (array passed by reference)
													// there is a significant speed-up but using
													// Vector the code is more "clean"


		inStream >> temp;
		if(temp.compare(";")) {
			// TODO: exception
			logInstance->Log("Error expected ';': found '" + temp + "'", 2);
		}

		inStream.clear();
		i++;
	}

	logInstance->Log("Finished read", 3);


	int tempsum=0;
	for(int j=0; j<Ndescriptors; j++) {
		vector<int> asd = list[j]->getDescriptorVector();
		for(int k=0; k<DescriptorsDimensionality; k++) {
			tempsum+=asd[k];
		}
		// cout << "asd: " << tempsum << endl;
		// cout << "asd: " << list[j]->getDescriptorVector()[0] << endl;
	}


	//cout << "difference: " << list[0]->compare(list[1]) << endl;

	return list;
}
