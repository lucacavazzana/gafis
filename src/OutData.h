#ifndef OUTDATA_H_
#define OUTDATA_H_

#include <string>

using namespace std;

class OutData {

public:
	OutData(int h, int w, int num, int ext);
	OutData(string file, int h, int w, int num, int ext);
	virtual ~OutData();

	string getName();

	string getScript();

	const static int PICT = 0;			// pict-file on macs, bmp-file on win (default)
	const static int PSD = 1;			// single layer Photoshop file, 48bits supported
	const static int PNG = 2;			// png-format, 48bits supported
	const static int TIFF = 3; 			// tiff-format, 48bits supported
	const static int PSD_mask = 4;		// Photoshop file, one image per layer + shape mask & feathered clip mask at overlap center
	const static int PSD_nomask = 5;	// Photoshop file, one image per layer, alpha layer with feathered clip mask at overlap center
	const static int TIFF_m = 6;		// tiff-format, multi-file, one image per file, 48bit supported alpha layer with non-feathered clip mask at image border + shape mask & non-feathered clip mask at image border
	const static int JPEG = 7;			// Panoramic image in jpeg-format. Use with f1 for IBM Hotmedia panoramas.
	const static int PAN = 8;			// SmoothMove movie. Use only with f2.
	const static int IVR = 9;			// LivePicture IVR movie cylindrical (format f1) or spherical (format f2)
	const static int IVR_java = 10;		// LivePicture Java Panorama, cylindrical (format f1) or spherical (format f2)
	const static int VRML = 11;			// VRML background node, use only with f2 for panoramas, or VRML-object for PTStereo
	const static int QTVR = 12;			// Apple QTVR-panomovie. Use only with f1


private:
#define PI (3.141592653589793)

	string name;		//file name
	int height;			//file height
	int width;			//file weight
	int format;			//file format
	double hofv;			//horizontal angle of view

	string GetFormat(int ext);
};

#endif /* OUTDATA_H_ */
