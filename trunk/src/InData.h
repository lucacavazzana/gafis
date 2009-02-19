#ifndef INDATA_H_
#define INDATA_H_

#include <string>
#include <Logger.h>

using namespace std;

class InData {

public:
	InData(string file, int i);
	virtual ~InData();
	void print(void);

	string getScript();		//returns input's script line
	void setHeight(int h);
	void setWidth(int w);
	int getHeight();
	int getWidth();
	double getHofv();


private:
#define PI (3.141592653589793)

	Logger * logger;

	string path;	//path
	int imgNum;		//position in the serie of images
	int height;		//pixel height
	int width;		//pixel width
	double hofv;	//horizontal angle of view
	int y;		//yaw
	int p;		//pitch
	int r;		//roll
	int a;		//lens distortion
	int b;		//lens distortion
	int c;		//lens distortion

	bool getImageSize();
};

#endif /* INDATA_H_ */
