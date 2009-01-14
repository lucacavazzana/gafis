#ifndef INTEGRAL_H
#define INTEGRAL_H


//! Computes the integral image of image img.  Assumes source image to be a 
//! 32-bit floating point.  Returns IplImage in 32-bit float form.
IplImage *Integral(IplImage *img);


//! Computes the sum of pixels within the rectangle specified by the top-left start
//! co-ordinate (x,y) with width w and height h.
float Area(IplImage *img, int x, int y, int w, int h);


#endif