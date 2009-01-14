#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "utils.h"
#include <iostream>
#include <fstream>
#include <time.h>
using namespace std;

//-------------------------------------------------------

//! Display error message and terminate program
void error(char *msg)
{
  cout << "\nError: " << msg;
  getchar();
  exit(0);
}

//-------------------------------------------------------

//! Show the provided image and wait for keypress
void showImage(IplImage *img)
{
  cvNamedWindow("Surf", CV_WINDOW_AUTOSIZE);
  cvShowImage("Surf", img);
  cvWaitKey(0);
}

//-------------------------------------------------------

//! Show the provided image in titled window and wait for keypress
void showImage(char *title, IplImage *img)
{
  cvNamedWindow(title, CV_WINDOW_AUTOSIZE);
  cvShowImage(title, img);
  cvWaitKey(0);
}

//-------------------------------------------------------

// Convert image to single channel 32F
IplImage* getGray(IplImage *img)
{
  IplImage* gray8, * gray32;

  gray8 = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );
  gray32 = cvCreateImage( cvGetSize(img), IPL_DEPTH_32F, 1 );

  if( img->nChannels == 1 )
    gray8 = (IplImage *) cvClone( img );
  else
    cvCvtColor( img, gray8, CV_BGR2GRAY );

  cvConvertScale( gray8, gray32, 1.0 / 255.0, 0 );

  cvReleaseImage( &gray8 );
  return gray32;
}

//-------------------------------------------------------

//! Draw all the Ipoints in the provided vector
void drawIpoints(IplImage *img, vector<Ipoint> &ipts, int tailSize)
{
  Ipoint *ipt;
  float s, o;
  int r1, c1, r2, c2, lap;

  for(unsigned int i = 0; i < ipts.size(); i++)
  {
    ipt = &ipts.at(i);
    s = ((9.0f/1.2f) * ipt->scale) / 3.0f;
    o = ipt->orientation;
    lap = ipt->laplacian;
    r1 = cvRound(ipt->y);
    c1 = cvRound(ipt->x);
    c2 = cvRound(s * cos(o)) + c1;
    r2 = cvRound(s * sin(o)) + r1;

    // Green line indicates orientation
    cvLine(img, cvPoint(c1, r1), cvPoint(c2, r2), cvScalar(0, 255, 0));

    if (lap >= 0)
    { // Blue circles indicate light blobs on dark backgrounds
      cvCircle(img, cvPoint(c1,r1), cvRound(s), cvScalar(255, 0, 0),1);
    }
    else
    { // Red circles indicate light blobs on dark backgrounds
      cvCircle(img, cvPoint(c1,r1), cvRound(s), cvScalar(0, 0, 255),1);
    }

    // Draw motion from ipoint dx and dy
    if (tailSize)
    {
      cvLine(img, cvPoint(c1,r1),
        cvPoint(int(c1+ipt->dx*tailSize), int(r1+ipt->dy*tailSize)),
        cvScalar(255,255,255), 1);
    }
  }
}

//-------------------------------------------------------

//! Draw a single feature on the image
void drawIpoint(IplImage *img, Ipoint &ipt)
{
  float s, o;
  int r1, c1, r2, c2, lap;

  s = ((9.0f/1.2f) * ipt.scale) / 3.0f;
  s*=3;
  o = ipt.orientation;
  lap = ipt.laplacian;
  r1 = cvRound(ipt.y);
  c1 = cvRound(ipt.x);
  c2 = cvRound(s * cos(o)) + c1;
  r2 = cvRound(s * sin(o)) + r1;

  // Green line indicates orientation
  cvLine(img, cvPoint(c1, r1), cvPoint(c2, r2), cvScalar(0, 255, 0));

  if (lap >= 0)
  { // Blue circles indicate light blobs on dark backgrounds
    cvCircle(img, cvPoint(c1,r1), cvRound(s), cvScalar(255, 0, 0),1);
  }
  else
  { // Red circles indicate light blobs on dark backgrounds
    cvCircle(img, cvPoint(c1,r1), cvRound(s), cvScalar(0, 0, 255),1);
  }
}

//-------------------------------------------------------

//! Draw a single feature on the image
void drawPoint(IplImage *img, Ipoint &ipt)
{
  float s, o;
  int r1, c1;

  s = 4;
  o = ipt.orientation;
  r1 = cvRound(ipt.y);
  c1 = cvRound(ipt.x);

  cvCircle(img, cvPoint(c1,r1), cvRound(s), cvScalar(0, 255, 0), -1);
  cvCircle(img, cvPoint(c1,r1), cvRound(s+1), cvScalar(255, 0,0), 2);

}

//-------------------------------------------------------

//! Draw a single feature on the image
void drawPoints(IplImage *img, vector<Ipoint> &ipts)
{
  float s, o;
  int r1, c1;

  for(unsigned int i = 0; i < ipts.size(); i++)
  {
    s = 10;
    o = ipts[i].orientation;
    r1 = cvRound(ipts[i].y);
    c1 = cvRound(ipts[i].x);

    cvCircle(img, cvPoint(c1,r1), cvRound(s), cvScalar(0, 255, 0), -1);
    cvCircle(img, cvPoint(c1,r1), cvRound(s+1), cvScalar(255, 0,0), 2);
  }
}

//-------------------------------------------------------

//! Draw descriptor windows around Ipoints in the provided vector
void drawWindows(IplImage *img, vector<Ipoint> &ipts)
{
  Ipoint *ipt;
  float s, o, cd, sd;
  int x, y;
  CvPoint2D32f src[4];

  for(int i = 0; i < cvRound(ipts.size()); i++)
  {
    ipt = &ipts.at(i);
    s = (10 * ipt->scale);
    o = ipt->orientation;
    y = cvRound(ipt->y);
    x = cvRound(ipt->x);
    cd = cos(o);
    sd = sin(o);

    src[0].x=sd*s+cd*s+x;   src[0].y=-cd*s+sd*s+y;
    src[1].x=sd*s+cd*-s+x;  src[1].y=-cd*s+sd*-s+y;
    src[2].x=sd*-s+cd*-s+x; src[2].y=-cd*-s+sd*-s+y;
    src[3].x=sd*-s+cd*s+x;  src[3].y=-cd*-s+sd*s+y;

    // Draw orientation line
    cvLine(img, cvPoint(cvRound(x), cvRound(y)),
      cvPoint(cvRound(s*cd + x), cvRound(s*sd + y)), cvScalar(0, 255, 0),1);

    // Draw box window around the point
    cvLine(img, cvPoint(cvRound(src[0].x), cvRound(src[0].y)),
      cvPoint(cvRound(src[1].x), cvRound(src[1].y)), cvScalar(255, 0, 0),2);
    cvLine(img, cvPoint(cvRound(src[1].x), cvRound(src[1].y)),
      cvPoint(cvRound(src[2].x), cvRound(src[2].y)), cvScalar(255, 0, 0),2);
    cvLine(img, cvPoint(cvRound(src[2].x), cvRound(src[2].y)),
      cvPoint(cvRound(src[3].x), cvRound(src[3].y)), cvScalar(255, 0, 0),2);
    cvLine(img, cvPoint(cvRound(src[3].x), cvRound(src[3].y)),
      cvPoint(cvRound(src[0].x), cvRound(src[0].y)), cvScalar(255, 0, 0),2);

  }
}

//-------------------------------------------------------

// Draw the FPS figure on the image (requires at least 2 calls)
void drawFPS(IplImage *img)
{
  static clock_t t;
  char fps_text[20];
  CvFont font;
  cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, 1.0,1.0,0,2);

  // Add fps figure
  float fps = (1.0f/(clock()-t) * CLOCKS_PER_SEC);

  // Update variables
  t=clock();

  // Get the figure as a string
  sprintf(fps_text,"FPS: %.2f",fps);

  // Draw the string on the image
  cvPutText (img,fps_text,cvPoint(10,25), &font, cvScalar(255,255,0));
}

//-------------------------------------------------------

//! Save the SURF features to file
void saveSurf(char *filename, vector<Ipoint> &ipts)
{
  ofstream outfile(filename);

  // output descriptor length
  outfile << "64\n";
  outfile << ipts.size() << "\n";

  // create output line as:  scale  x  y  des
  for(unsigned int i=0; i < ipts.size(); i++)
  {
    outfile << ipts.at(i).scale << "  ";
    outfile << ipts.at(i).x << " ";
    outfile << ipts.at(i).y << " ";
    for(int j=0; j<64; j++)
      outfile << ipts.at(i).descriptor[j] << " ";

    outfile << "\n";
  }

  outfile.close();
}

//-------------------------------------------------------

//! Load the SURF features from file
void loadSurf(char *filename, vector<Ipoint> &ipts)
{
  int count;
  float temp;
  ifstream infile(filename);

  // output descriptor length
  infile >> count;
  infile >> count;

  for (int i = 0; i < count; i++)
  {
    Ipoint ipt;

    infile >> ipt.x;
    infile >> ipt.y;
    infile >> temp;
    ipt.scale = temp;
    ipt.laplacian = 1;

    for (int j = 0; j < 3; j++)
      infile >> temp;

    for (int j = 0; j < 64; j++)
      infile >> ipt.descriptor[j];

    ipts.push_back(ipt);

  }
}
