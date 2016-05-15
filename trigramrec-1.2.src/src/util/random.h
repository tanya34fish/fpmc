/*
	Sampling methods

	Author:   Steffen Rendle, http://www.libfm.org/
	modified: 2011-07-14

	Copyright 2010-2011 Steffen Rendle, see license.txt for more information
*/

#ifndef RANDOM_H_
#define RANDOM_H_

#include <stdlib.h>
#include <cmath>

double ran_gaussian();
double ran_gaussian(double mean, double stdev);
double ran_uniform();
double ran_exp();			

double ran_gaussian() {
	// method from Joseph L. Leva: "A fast normal Random number generator"
	double u,v, x, y, Q;
	do {
		do {
			u = ran_uniform();
		} while (u == 0.0); 
		v = 1.7156 * (ran_uniform() - 0.5);
		x = u - 0.449871;
		y = std::abs(v) + 0.386595;
		Q = x*x + y*(0.19600*y-0.25472*x);
		if (Q < 0.27597) { break; }
	} while ((Q > 0.27846) || ((v*v) > (-4.0*u*u*std::log(u)))); 
	return v / u;
}

double ran_gaussian(double mean, double stdev) {
	if ((stdev == 0.0) || (isnan(stdev))) {
		return mean;
	} else {
		return mean + stdev*ran_gaussian();
	}
}

double ran_uniform() {
	return rand()/((double)RAND_MAX + 1);
}

double ran_exp() {
	return -log(1-ran_uniform());
}


#endif /*RANDOM_H_*/
