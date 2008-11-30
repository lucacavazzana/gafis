/*
 * Logger.cpp
 *
 *  Created on: 24-nov-2008
 *      Author: stefanocadario
 */

#include "Logger.h"
#include <iostream>
#include <string>
using namespace std;

Logger* Logger::classInstance = NULL;

Logger::Logger() {
	Logger(2);
}

Logger::Logger(int lvl) {
	level = lvl;
}

Logger::~Logger() {

}

void Logger::Log(std::string message, int message_level) {
	if(message_level<=level) {
		time (&rawtime);
		timeinfo = localtime ( &rawtime );
		string timestamp = asctime(timeinfo);
		cout << timestamp.erase(timestamp.length()-1) << ": " << message << "\n";
		cout.flush();
	}
}

void Logger::printMessage(std::string message) {
	std::cout << message << endl;
}

void Logger::setLevel(int lvl) {
	level = lvl;
}

Logger* Logger::getInstance() {
	if(!classInstance)
		classInstance = new Logger(2);
	return classInstance;
}
