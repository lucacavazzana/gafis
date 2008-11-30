/*
 * ColorDescGenerator.h
 *
 *  Created on: 24-nov-2008
 *  Author: stefanocadario
 */

#ifndef COLORDESCGENERATOR_H_
#define COLORDESCGENERATOR_H_

#include "DescGenerator.h"
#include "ImageDescriptor.h"
#include <iostream>
#include <vector>

using namespace std;

class ColorDescGenerator: public DescGenerator {
public:
	ColorDescGenerator();
	virtual ~ColorDescGenerator();
	vector<ImageDescriptor*> generateDescriptors();
protected:
	string bin_path;
	vector<ImageDescriptor*> textFileParser(string filepath);
};

#endif /* COLORDESCGENERATOR_H_ */
