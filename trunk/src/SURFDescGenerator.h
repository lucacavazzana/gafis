/*
 * SURFDescGenerator.h
 *
 *  Created on: 14-gen-2009
 *      Author: stefanocadario
 */

#ifndef SURFDESCGENERATOR_H_
#define SURFDESCGENERATOR_H_

#include "DescGenerator.h"

class SURFDescGenerator: public DescGenerator {
public:
	SURFDescGenerator();
	virtual ~SURFDescGenerator();
	vector<ImageDescriptor*> SURFDescGenerator::generateDescriptors();
};

#endif /* SURFDESCGENERATOR_H_ */
