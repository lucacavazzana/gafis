//immagazzina tutte le informazioni necessarie relative ad ogni singola immagine

#include "InData.h"
#include <iostream>
#include <sstream>
#include <exif.h>
#include <exif.cpp>
#include <stdlib.h>
#include <math.h>


using namespace std;

InData::InData(string file, int num){
	logger = Logger::getInstance();
	hofv = 0;

	EXIFINFO m_exifinfo;
	memset(&m_exifinfo,0,sizeof(EXIFINFO));
	Cexif exif(&m_exifinfo);

	path = file;
	imgNum = num;

	FILE* file_handler = fopen(file.c_str(), "r");
	if(exif.DecodeExif(file_handler)){
		logger->Log("exif trovata",3);
		height = m_exifinfo.Height;
		width = m_exifinfo.Width;

//		tag exif troppo inaffidabili
//		if (width > height)
//			hofv =  (180/PI)*2*atan(m_exifinfo.CCDWidth/m_exifinfo.FocalLength);
//		else
//			hofv =  (180/PI)*2*atan(m_exifinfo.CCDWidth*(m_exifinfo.Height/m_exifinfo.Width)/m_exifinfo.FocalLength);

	}



	getImageSize();  //TODO: aggiungere controllo errore nel caso manchi l'immagine (anche se dovrebbe già esistere, visto che ci abbiamo calcolato i punti di controllo)

	if (hofv==0)
		hofv = 50; //50 reasonable value for pocket cameras... keep this value if can't access to exif tags

	if(num == 0){ //prima immagine presa come riferimento per le altre, ha orientamento [0,0,0]
		y = 0;
		p = 0;
		r = 0;
	}
	else { //per le altre immagini potrebbero essere disponibili suggerimenti sull'orientamento iniziale
		y = 0;
		p = 0;
		r = 0;
	}

	a = 0;
	b = 0;
	c = 0;
}

InData::~InData(){}

string InData::getScript(){
	stringstream tmp;
	string script = "i";

	script += " f0";		// rectilinear image

	tmp << width;
	script += " w" + tmp.str();

	tmp.str("");
	tmp << height;
	script += " h" + tmp.str();

	tmp.str("");
	tmp << hofv;
	script += " v" + tmp.str();

	tmp.str("");
	tmp << y;
	script += " y" + tmp.str();

	tmp.str("");
	tmp << p;
	script += " p" + tmp.str();

	tmp.str("");
	tmp << r;
	script += " r" + tmp.str();

	script += " n\"" + path + "\" o";

	return script;
}

int InData::getHeight(){
	return height;
}

int InData::getWidth(){
	return width;
}

double InData::getHofv(){
	return hofv;
}

void InData::print(void){
	cerr << path << endl;
}

//bool InData::GetImageSize(const char *fn, int *x,int *y){
bool InData::getImageSize(){
	FILE *f = fopen(path.c_str(),"rb");
	if (f==0)
		return false;

	fseek(f,0,SEEK_END);
	long len=ftell(f);
	fseek(f,0,SEEK_SET);

	if (len<24) {
		fclose(f);
		return false;
	}

	// Strategy:
	// reading GIF dimensions requires the first 10 bytes of the file
	// reading PNG dimensions requires the first 24 bytes of the file
	// reading JPEG dimensions requires scanning through jpeg chunks
	// In all formats, the file is at least 24 bytes big, so we'll read that always
	unsigned char buf[24];
	fread(buf,1,24,f);

	// For JPEGs, we need to read the first 12 bytes of each chunk.
	// We'll read those 12 bytes at buf+2...buf+14, i.e. overwriting the existing buf.
	if (buf[0]==0xFF && buf[1]==0xD8 && buf[2]==0xFF && buf[3]==0xE0 && buf[6]=='J' && buf[7]=='F' && buf[8]=='I' && buf[9]=='F') {
		long pos=2;
		while (buf[2]==0xFF){

			if (buf[3]==0xC0 || buf[3]==0xC1 || buf[3]==0xC2 || buf[3]==0xC3 || buf[3]==0xC9 || buf[3]==0xCA || buf[3]==0xCB)
				break;

			pos += 2+(buf[4]<<8)+buf[5];

			if (pos+12>len)
				break;

			fseek(f,pos,SEEK_SET);
			fread(buf+2,1,12,f);
		}
	}

	fclose(f);

	// JPEG: (first two bytes of buf are first two bytes of the jpeg file; rest of buf is the DCT frame
	if (buf[0]==0xFF && buf[1]==0xD8 && buf[2]==0xFF) {
		height = (buf[7]<<8) + buf[8];
		width = (buf[9]<<8) + buf[10];
		return true;
	}

	// GIF: first three bytes say "GIF", next three give version number. Then dimensions
	if (buf[0]=='G' && buf[1]=='I' && buf[2]=='F') {
		width = buf[6] + (buf[7]<<8);
		height = buf[8] + (buf[9]<<8);
		return true;
	}

	// PNG: the first frame is by definition an IHDR frame, which gives dimensions
	if ( buf[0]==0x89 && buf[1]=='P' && buf[2]=='N' && buf[3]=='G' && buf[4]==0x0D && buf[5]==0x0A && buf[6]==0x1A && buf[7]==0x0A && buf[12]=='I' && buf[13]=='H' && buf[14]=='D' && buf[15]=='R') {
		width = (buf[16]<<24) + (buf[17]<<16) + (buf[18]<<8) + (buf[19]<<0);
		height = (buf[20]<<24) + (buf[21]<<16) + (buf[22]<<8) + (buf[23]<<0);
		return true;
	}

	return false;
}

void InData::setHeight(int h){
	height = h;
}
void InData::setWidth(int w){
	width = w;
}
