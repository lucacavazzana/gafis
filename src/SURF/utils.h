#ifndef UTILS_H
#define UTILS_H

#include <opencv/cv.h>
#include "ipoint.h"
#include <vector>


//! Display error message and terminate program
void error(char *msg);

//! Show the provided image and wait for keypress
void showImage(IplImage *img);

//! Show the provided image in titled window and wait for keypress
void showImage(char *title, IplImage *img);

// Convert image to single channel 32F
IplImage* getGray(IplImage *img);

//! Draw a single feature on the image
void drawIpoint(IplImage *img, Ipoint &ipt);

//! Draw all the Ipoints in the provided vector
void drawIpoints(IplImage *img, std::vector<Ipoint> &ipts, int tailSize = 0);

//! Draw descriptor windows around Ipoints in the provided vector
void drawWindows(IplImage *img, std::vector<Ipoint> &ipts);

// Draw the FPS figure on the image (requires at least 2 calls)
void drawFPS(IplImage *img);

//! Draw a Point at feature location
void drawPoint(IplImage *img, Ipoint &ipt);

//! Draw a Point at all features
void drawPoints(IplImage *img, std::vector<Ipoint> &ipts);

//! Save the SURF features to file
void saveSurf(char *filename, std::vector<Ipoint> &ipts);

//! Load the SURF features from file
void loadSurf(char *filename, std::vector<Ipoint> &ipts);

#endif
