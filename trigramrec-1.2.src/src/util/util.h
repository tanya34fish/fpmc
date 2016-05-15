/*
	Utility functions

	Author:   Steffen Rendle, http://www.libfm.org/
	modified: 2011-07-18

	Copyright 2010-2011 Steffen Rendle, see license.txt for more information
*/

#ifndef UTIL_H_
#define UTIL_H_

#include <vector>
#include <ctime>
#include <sys/resource.h>

typedef unsigned int uint;

double sqr(double d) { return d*d; }


std::vector<std::string> tokenize(const std::string& str, const std::string& delimiter) {
	std::vector<std::string> result;
	std::string::size_type lastPos = str.find_first_not_of(delimiter, 0);

	std::string::size_type pos = str.find_first_of(delimiter, lastPos);
	while (std::string::npos != pos || std::string::npos != lastPos) {
		result.push_back(str.substr(lastPos, pos - lastPos));
        	lastPos = str.find_first_not_of(delimiter, pos);
        	pos = str.find_first_of(delimiter, lastPos);
	}
	return result;
}

double getusertime() { 
	struct rusage ru;        
	getrusage(RUSAGE_SELF, &ru);        
  
	struct timeval tim = ru.ru_utime;        
	return (double)tim.tv_sec + (double)tim.tv_usec / 1000000.0; 
}   

#endif /*UTIL_H_*/
