#include <OutData.h>
#include <iostream>
#include <sstream>
#include <math.h>

using namespace std;

// in: altezza, larghezza, formato desiderato
OutData::OutData(int h, int w, int num, int ext){
	name = "output";
	width = w;
	height = h;
	format = ext;

	hofv = (180/PI)*2*atan((num-1)*2*tan(25*PI/180) + tan(25*PI/180)); // 25 = 1/2 hofv tipico di un'immagine da pocket camera (50). Eventualmente modificare in modo da considerare hofv effettivo se specificato.
}

//in: nome file, altezza, larghezza, formato desiderato
OutData::OutData(string file, int h, int w, int num, int ext){
	name = file;
	width = w;
	height = h;
	format = ext;

	//hofv = num*50;
	hofv = (180/PI)*2*atan((num-1)*2*tan(25*PI/180) + tan(25*PI/180)); // 25 = 1/2 hofv tipico di un'immagine da pocket camera (50). Eventualmente modificare in modo da considerare hofv effettivo se specificato.

}

string OutData::getName(){
	return name;
}

OutData::~OutData(){}

string OutData::getScript(){
	stringstream tmp;
	string script = "p";

	script += " f0";

	tmp << width;
	script += " w" + tmp.str();

	tmp.str("");
	tmp << height;
	script += " h" + tmp.str();

	tmp.str("");
	tmp << hofv;
	script += " v" + tmp.str();

	script += " n" + GetFormat(format);

	return script;
}

string OutData::GetFormat(int ext){
	if (ext==PICT)
		return "PICT";
	if(ext==PSD)
		return "PSD";
	if (ext==PNG)
		return "PNG";
	if (ext==TIFF)
		return "TIFF";
	if (ext==PSD_mask)
		return "PSD_mask";
	if (ext==PSD_nomask)
		return "PSD_nomask";
	if (ext==TIFF_m)
		return "TIFF_m";
	if (ext==JPEG)
		return "JPEG";
	if (ext==PAN)
		return "PAN";
	if (ext==IVR)
		return "IVR";
	if (ext==IVR_java)
		return "IVR_java";
	if (ext==VRML)
		return "VRML";
	if (ext==QTVR)
		return "QTVR";
	return "JPEG";
}
