/*
 * GASAC.cpp
 *
 *  Created on: 19-gen-2009
 *      Author: stefanocadario
 */

#include "GASAC.h"
#include <iostream>
#include <math.h>
#include <ga/ga.h>
#include <PointCorrispondence.h>

using namespace std;

GASAC* GASAC::classInstance = NULL; // TODO: why this must be here ???

GASAC::GASAC(PointCorrispondence *corr, int size) {
	corrispondence = corr;
	ncorrispondence = size;
	geneSize = 8;
}

GASAC::GASAC() {
	geneSize = 8;
	popsize= 100;
	ngen   = 100;
	pmut   = 1/(2*geneSize);
	pmut   = 0.1;
	//pmut = 0;
	pcross = 0.8;
}

GASAC* GASAC::getInstance() {
	if(!classInstance)
		classInstance = new GASAC();
	return classInstance;
}

void GASAC::setPoints(PointCorrispondence *corr, int size) {
	corrispondence = corr;
	ncorrispondence = size;
}

float GASAC::Objective(GAGenome& g)
{
	GA1DArrayGenome<int> & genome = (GA1DArrayGenome<int> &)g;
	GASAC *instance = GASAC::getInstance();

	// Build Foundamental Matrix
	for( int i = 0; i < instance->geneSize; i++ )
	{
		instance->points1->data.fl[i*2]   = instance->corrispondence[genome.gene(i)].p1.x;
		instance->points1->data.fl[i*2+1] = instance->corrispondence[genome.gene(i)].p1.y;
		instance->points2->data.fl[i*2]   = instance->corrispondence[genome.gene(i)].p2.x;
		instance->points2->data.fl[i*2+1] = instance->corrispondence[genome.gene(i)].p2.y;
	}
	int fm_count = cvFindFundamentalMat(instance->points1, instance->points2,
		                                     instance->fundamental_matrix,
		                                     CV_FM_8POINT,1.0,0.99,instance->status);

	// Check what point respect the calculated foundamental matrix

	// Compute with robust method "Median"
	float values[instance->ncorrispondence];
	float threshold =5;
	float threshold2=threshold*threshold;
	float sum =0;
	for(int i=0; i<instance->ncorrispondence; i++) {
			values[i] = instance->corrispondence[i].computeError(instance->fundamental_matrix);
			//cout << "asd: " << values[i]<< endl;
			if(values[i]<threshold) {
				//cout << "asd: " << values[i]<< endl;
				//values[i] = threshold;
				//cout << "lol" << endl;
				//sum+=values[i]*pow(threshold2-pow(values[i],2),2);
			}
			sum+=values[i];
	}

	// Metodo somma
	//return sum;
	// Metodo mediana
	sort(values, values + instance->ncorrispondence);
	float mediana = values[instance->ncorrispondence/2];
	return mediana;
	return 1.96*(1+5/(instance->ncorrispondence-1))*mediana;
	//return values[instance->ncorrispondence/2];
	/*
	float sum =0;
	for(int i=0; i<instance->ncorrispondence; i++) {
		sum += instance->corrispondence->computeError(instance->fundamental_matrix);
	}
	cout << "Sum: " << sum << endl;
	return sum;
	*/
}

void GASAC::init(GAGenome& c) {
	GA1DArrayGenome<int> & genome = (GA1DArrayGenome<int> &)c;
	GASAC *instance = GASAC::getInstance();
	int candidate = 0;
    for(int d = 0; d < instance->geneSize; d++) {
    	bool exist;
    	do {
    		exist = false;
    		candidate = GARandomInt(0,instance->ncorrispondence-1);
    		for(int i=0; i<instance->geneSize; i++)
    			if(genome.gene(i)==candidate) // TODO: can break for loop
    				exist = true;
    	} while(exist);
    	genome.gene(d, candidate);
	}
}

int GASAC::mutate(GAGenome& c, float mutprob)
{
	GA1DArrayGenome<int>& genome = (GA1DArrayGenome<int>&)c;
	GASAC *instance = GASAC::getInstance();

	if(mutprob <= 0.0)
	return(0);

	int mutate = false;
	for(int d = 0; d < instance->geneSize; d++) {
		float mut = GARandomFloat(0.0, 1.0);
		if(mut < mutprob) {
			mutate = true;
			// Generate new element...check if already exist
			bool exist;
			int newvalue;
			do {
				exist = false;
				newvalue = GARandomInt(0, instance->ncorrispondence-1);
				for(int i=0; i<instance->geneSize; i++) {
					if(genome.gene(i)==newvalue) { // TODO: can break for loop
						exist = true;
						break;
					}
				}
			} while(exist);
			genome.gene(d, newvalue);
		}
	}
	return(mutate);
}

int crossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2) {
	GA1DArrayGenome<int> &mom=(GA1DArrayGenome<int> &)p1;
	GA1DArrayGenome<int> &dad=(GA1DArrayGenome<int> &)p2;

	int n=0;
	unsigned int site = GARandomInt(0, mom.length());
	unsigned int len = mom.length() - site;

	int a, b;
	bool exist;

	if(c1&&c2) {
		GA1DArrayGenome<int> &sis=(GA1DArrayGenome<int> &)*c1;
		GA1DArrayGenome<int> &bro=(GA1DArrayGenome<int> &)*c2;
		// Recopy first part of array
		sis.copy(mom, 0, 0, site);
		bro.copy(dad, 0, 0, site);
		//sis.copy(dad, site, site, len);
		for(int i=site; i<mom.length(); i++) {
			a = mom.gene(i);
			b = dad.gene(i);
			exist = false;
			for(int j=0; j<=site; j++) {
				if(a==bro.gene(j) || b==sis.gene(j)) {
					exist = true;
					break;
				}
			}
			if(!exist) {
				sis.gene(i,b);
				bro.gene(i,a);
			}
			else {
				sis.gene(i, a);
				bro.gene(i, b);
			}
		}
		n++;
	}
	return n;
}

float GASAC::comparator(const GAGenome& a, const GAGenome& b) {
	GA1DArrayGenome<int>& genome1 = (GA1DArrayGenome<int>&)a;
	GA1DArrayGenome<int>& genome2 = (GA1DArrayGenome<int>&)b;
	if(genome1.length() != genome2.length()) return -1;
	float count = 0.0;
	for(int i=genome1.length()-1; i>=0; i--) {
		for(int j=0; j<genome2.length(); j++) {
			if(genome1.gene(i)==genome2.gene(j))
				count++;
		}
	}
	return genome1.length() - count;
}


int GASAC::run() {

	// Create base structure used during evolution
	points1 = cvCreateMat(1,geneSize,CV_32FC2);
	points2 = cvCreateMat(1,geneSize,CV_32FC2);
	status = cvCreateMat(1,geneSize,CV_8UC1);
	fundamental_matrix = cvCreateMat(3,3,CV_32FC1);

	GA1DArrayGenome<int> genome(geneSize, Objective);

	genome.initializer(init);
	genome.mutator(mutate);
	genome.comparator(comparator);
	genome.crossover(crossover);
	// Now create the GA using the genome, set the parameters, and run it.
	GASimpleGA ga(genome);

	GANoScaling noscale;
	ga.minimize();
	ga.scaling(noscale);
	ga.populationSize(popsize);
	ga.nGenerations(ngen);
	ga.pMutation(pmut);
	ga.pCrossover(pcross);
	//ga.scoreFilename("bog.dat");
	//ga.flushFrequency(50);	// dump scores to disk every 50th generation

	ga.evolve();

	// Dump the results of the GA to the screen.  We print out first what a random
	// genome looks like (so we get a bit of a feel for how hard it is for the
	// GA to find the right values) then we print out the best genome that the
	// GA was able to find.

	genome.initialize();

	cout << "random values in the genome:\n";

	for(int i=0; i<geneSize; i++){
		cout.width(10); cout << genome.gene(i) << " ";
	}
	cout << "\n";

	ga.minimize();

	genome = ga.statistics().bestIndividual();
	cout << "the ga generated:\n" << ga.statistics().bestIndividual() << endl;

	bestCouples = new int[geneSize];
	int j=0;
	for(int i=0; i<geneSize; i++) {
		bestCouples[j++] = genome.gene(i);
		corrispondence[genome.gene(i)].printPoints();
		cout << " Diff: " << corrispondence[genome.gene(i)].getDifference() << endl;
		//cout << "\n";
	}

	cout << "\n\n"; cout.flush();

}

GASAC::~GASAC() {
	// TODO Auto-generated destructor stub
}
