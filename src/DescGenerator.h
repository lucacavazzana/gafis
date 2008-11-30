/*
 * DescGenerator.h
 *
 *  Created on: 24-nov-2008
 *  Author: stefanocadario
 */

#ifndef DESCGENERATOR_H_
#define DESCGENERATOR_H_

#include "ImageDescriptor.h"
#include <iostream>
#include <vector>
#include <Logger.h>
using namespace std;

class DescGenerator {
public:
	DescGenerator();
	virtual ~DescGenerator();
	virtual vector<ImageDescriptor*> generateDescriptors() { };
	void setImagePath(string path);
protected:
	string imagePath;
	Logger* logInstance;
};

#endif /* DESCGENERATOR_H_ */
