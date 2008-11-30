/*
 * Logger.h
 *
 *  Created on: 24-nov-2008
 *      Author: stefanocadario
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "time.h"
#include <iostream>
#include <string>

class Logger {
public:
	static Logger* getInstance();
	void Log(std::string message, int message_level = 2);
	void printMessage(std::string message);
	void setLevel(int lvl);
	virtual ~Logger();
protected:
	Logger();
	//Logger(Logger const&) {};
	Logger(int lvl);
	//Logger& operator=(Logger const&){};
	static Logger* classInstance;
	int level;
	time_t rawtime;
	struct tm * timeinfo;
};

#endif /* LOGGER_H_ */
