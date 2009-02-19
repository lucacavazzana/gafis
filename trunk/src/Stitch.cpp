#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <InData.h>
#include <OutData.h>
#include <Logger.h>
#include <Optimizing.h>
#include <PointCorrispondence.h>

/* DEBUG_LVL = Livello di debug
 * definisce il livello di "verbose" dell'applicazione
 * 0 = Nessun messaggio in output di debug
 * 1 = Solo messaggi di errore in output
 * 2 = Messaggi informativi e di errore in output
 * 3 = Messaggi informativi, di errore e warning
 * 4 = Massimo livello possibile di debug
 *
 * NON TI PARE D'AVERLO GIA' LETTO DA QUALCHE PARTE? */
#define DEBUG_LVL 3

#define MAX_IMAGES 2

#define NCOUPLES 6

using namespace std;

// Global Variables

/*************VARIABILI FUFFA************************
 * già presenti nel codice di Gafis*****************/
Logger *logger;

string images[MAX_IMAGES]; // images' path
int nimages=0;
string outputImg; //output image filename

/**************************************************
 *************************************************/




InData *input[MAX_IMAGES];		// stores images' data
OutData *output;				// output data


int main (){




/***********************************************************************************
****************************SERIE DI VARIABILI FUFFA*******************************
****questi dovrebbero essere già tutti quanti presenti nel main di gafis***********
***********************************************************************************/
	logger = Logger::getInstance();
	logger->setLevel(DEBUG_LVL);
	nimages = 2;
	images[0] = "scr01.JPG";
	images[1] = "scr02.JPG";

	PointCorrispondence corrispondence[NCOUPLES]; //qui solo 5 (è dura farseli a mano...)
#define MAXDOUBLE 10000
	for(int i=0; i<NCOUPLES; i++)
		corrispondence[i].setDifference(MAXDOUBLE);

	corrispondence[0].setPoints(1622, 1261, 338, 1162);
	corrispondence[1].setPoints(1703, 578, 365, 583);
	corrispondence[2].setPoints(1653, 860, 356, 824);
	corrispondence[3].setPoints(1320, 455, 30, 512);
	corrispondence[4].setPoints(1910, 901, 573, 835);
	corrispondence[5].setPoints(2018, 1509, 663, 1319);

	/*
	//land
	corrispondence[0].setPoints(2811, 538, 2280, 440);
	corrispondence[1].setPoints(2037, 1457, 1508, 1325);
	corrispondence[2].setPoints(3100, 1459, 2535, 1333);
	corrispondence[3].setPoints(2890, 1720, 2330, 1590);
	corrispondence[4].setPoints(1747, 1064, 1229, 930);
	//img
	corrispondence[0].setPoints(717, 1839, 2371, 1666);
	corrispondence[1].setPoints(784, 1500, 2458, 1341);
	corrispondence[2].setPoints(1213, 2390, 2868, 2305);
	corrispondence[3].setPoints(622, 706, 2433, 530);
	corrispondence[4].setPoints(600, 2294, 2258, 2086);
	corrispondence[5].setPoints(166, 2032, 1832, 1755);
	//tv
	corrispondence[0].setPoints(1969, 935, 2150, 674);
	corrispondence[1].setPoints(1952, 1672, 2112, 1397);
	corrispondence[2].setPoints(892, 865, 773, 763);
	corrispondence[3].setPoints(2861, 1470, 2840, 1023);
	corrispondence[4].setPoints(1708, 1642, 1865, 1432);
	//again
	corrispondence[0].setPoints(2383, 1425, 2002, 1558);
	corrispondence[1].setPoints(2253, 137, 1770, 225);
	corrispondence[2].setPoints(1169, 834, 779, 983);
	corrispondence[3].setPoints(920, 606, 557, 781);
	corrispondence[4].setPoints(3146, 899, 2850, 961);
	//lib
	corrispondence[0].setPoints(1613, 1249, 1643, 330);
	corrispondence[1].setPoints(485, 978, 455, 15);
	corrispondence[2].setPoints(1088, 1109, 1069, 166);
	corrispondence[3].setPoints(1256, 1475, 1265, 557);
	corrispondence[4].setPoints(835, 1407, 828, 464);
	corrispondence[5].setPoints(411, 1476, 393, 543);
	//scr
	corrispondence[0].setPoints(1622, 1261, 338, 1162);
	corrispondence[1].setPoints(1703, 578, 365, 583);
	corrispondence[2].setPoints(1653, 860, 356, 824);
	corrispondence[3].setPoints(1320, 455, 30, 512);
	corrispondence[4].setPoints(1910, 901, 573, 835);
	corrispondence[5].setPoints(1378, 296, 66, 373);
	*/
/***********************************************************************************
***********************************************************************************
*********************************************************************************/

	outputImg = "output";	// desired output file name

	int i;
	int height, width;		// desired output dimensions

	string script_name = "script.txt";	// script's filename


	for(i=0; i<nimages; i++)
		input[i] = new InData(images[i], i);


	height = input[0]->getHeight();			// TODO: aggiungere modo per settarlo da linea di comando?
	width = input[0]->getWidth();
	output = new OutData(outputImg, height, width, nimages, OutData::JPEG);


	//print script file


	system(("rm -f " + script_name).c_str());

	ofstream script_file;
	script_file.open(script_name.c_str());

	script_file << output->getScript() << endl; 				//info file output
	for (i = 0; i<nimages; i++)									//info file input
		script_file << input[i]->getScript() << endl;
	script_file << Optimizing::getScript(nimages) << endl;		//dati da ottimizzare
	for(i=0; i<NCOUPLES; i++)
		script_file << corrispondence[i].getScript() << endl;	//associazioni control point
	//script_file << "m p1" << endl;
	script_file << "\n*";										//fine file

	script_file.close();

	logger->Log("Script stampato",3);
	logger->Log(output->getScript(),3);
	for (i = 0; i<nimages; i++)
		logger->Log((input[i])->getScript(),3);
	logger->Log(Optimizing::getScript(nimages),3);
	for(i=0; i<NCOUPLES; i++)
		logger->Log(corrispondence[i].getScript());
	logger->Log("",3);

	script_file.close();

	logger->Log("Lancio PTOptimizer",3);

	system(("./PTOptimizer.exe "+script_name).c_str());

	logger->Log("Lancio PTStitch",3);
	//system(("./PTStitcher.exe -o "+output->getName()+" "+script_name).c_str());
	system(("./PTStitcher.exe -o "+output->getName()+" "+script_name+" "+images[0]+" "+images[1]).c_str());
	logger->Log("già fatto?",3);

	return 0;
}
