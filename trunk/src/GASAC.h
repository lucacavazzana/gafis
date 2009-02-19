/*
 * GASAC.h
 *
 *  Created on: 19-gen-2009
 *      Author: stefanocadario
 */

#include <ga/ga.h>
#include <PointCorrispondence.h>

#ifndef GASAC_H_
#define GASAC_H_

class GASAC {
public:
	GASAC(PointCorrispondence *corr, int size);
	GASAC();
	virtual ~GASAC();
	void setPoints(PointCorrispondence *corr, int size);
	int run();

	static GASAC* getInstance();

	// GA Related
	static float Objective(GAGenome& g);
	static void init(GAGenome& c);
	static int mutate(GAGenome& c, float mutprob);
	static float comparator(const GAGenome& a, const GAGenome& b);

	// Variables & Parameters
	PointCorrispondence *corrispondence;
	int ncorrispondence;
	int geneSize;

	int popsize;
	int ngen;
	float pmut;
	float pcross;

	int *bestCouples;

protected:
	static GASAC* classInstance;

	CvMat* points1;
	CvMat* points2;
	CvMat* status;
	CvMat* fundamental_matrix;

};

#endif /* GASAC_H_ */
