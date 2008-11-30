/*
 * DescGenerator.cpp
 *
 *  Created on: 24-nov-2008
 *      Author: stefanocadario
 */

#include "DescGenerator.h"
#include <iostream>
using namespace std;

DescGenerator::DescGenerator() {
	// TODO: Do generic stuff like obtain reference for logging
	logInstance = Logger::getInstance();
}

DescGenerator::~DescGenerator() {

}

void DescGenerator::setImagePath(string path) {
	imagePath = path;
}
