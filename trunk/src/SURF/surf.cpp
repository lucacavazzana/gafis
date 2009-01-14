#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "integral.h"
#include "surf.h"
#include "utils.h"
#include <fstream>
#include <iostream>
using namespace std;

const float pi = float(CV_PI);

//! Destructor
Surf::~Surf()
{
}

//-------------------------------------------------------

//! Constructor
Surf::Surf(IplImage *img, IpVec &ipts)
: ipts(ipts)
{
  this->img = img;
}

//-------------------------------------------------------

//! Describe all features in the supplied vector
void Surf::getDescriptors(bool upright)
{
  // Check there are Ipoints to be described
  if (!ipts.size()) return;

  // Get the size of the vector for fixed loop bounds
  int ipts_size = ipts.size();

  if (upright)
  {
    // U-SURF loop just gets descriptors
    for (unsigned int i = 0; i < ipts_size; ++i)
    {
      // Set the Ipoint to be described
      index = i;

      // Extract upright (i.e. not rotation invariant) descriptors
      getUprightDescriptor();
    }
  }
  else
  {
    // Main SURF-64 loop assigns orientations and gets descriptors
    for (unsigned int i = 0; i < ipts_size; ++i)
    {
      // Set the Ipoint to be described
      index = i;

      // Assign Orientations and extract rotation invariant descriptors
      getOrientation();
      getDescriptor();
    }
  }
}

//-------------------------------------------------------

//! Assign the supplied Ipoint an orientation
void Surf::getOrientation()
{
  Ipoint *ipt = &ipts.at(index);
  float gauss, scale = ipt->scale;
  int s = cvRound(scale), r = cvRound(ipt->y), c = cvRound(ipt->x);
  std::vector<float> resX, resY, Ang;

  // calculate haar responses for points within radius of 6*scale
  for(int i = -6*s; i <= 6*s; i+=s)
  {
    for(int j = -6*s; j <= 6*s; j+=s)
    {
      if( i*i + j*j  < 36*s*s )
      {
        gauss = gaussian(i, j, 2.5f*s);
        resX.push_back( gauss * haarX(r+j, c+i, 4*s) );
        resY.push_back( gauss * haarY(r+j, c+i, 4*s) );
        Ang.push_back(getAngle(resX.back(), resY.back()));
      }
    }
  }

  // calculate the dominant direction
  float sumX, sumY;
  float max=0, old_max = 0, orientation = 0, old_orientation = 0;
  float ang1, ang2, ang;

  // loop slides pi/3 window around feature point
  for(ang1 = 0; ang1 < 2*pi;  ang1+=0.2f) {
    ang2 = ( ang1+pi/3.0f > 2*pi ? ang1-5.0f*pi/3.0f : ang1+pi/3.0f);
    sumX = sumY = 0;
    for(unsigned int k = 0; k < Ang.size(); k++)
    {
      // get angle from the x-axis of the sample point
      ang = Ang.at(k);

      // determine whether the point is within the window
      if (ang1 < ang2 && ang1 < ang && ang < ang2)
      {
        sumX+=resX.at(k);
        sumY+=resY.at(k);
      }
      else if (ang2 < ang1 &&
        ((ang > 0 && ang < ang2) || (ang > ang1 && ang < 2*pi) ))
      {
        sumX+=resX.at(k);
        sumY+=resY.at(k);
      }
    }

    // if the vector produced from this window is longer than all
    // previous vectors then this forms the new dominant direction
    if (sumX*sumX + sumY*sumY > max)
    {
      // store second largest orientation
      old_max = max;
      old_orientation = orientation;

      // store largest orientation
      max = sumX*sumX + sumY*sumY;
      orientation = getAngle(sumX, sumY);
    }
  }

  // check whether there are two dominant orientations based on 0.8 threshold
  if (old_max >= 0.8*max)
  {
    // assign second largest orientation and push copy onto vector
    ipt->orientation = old_orientation;
    ipts.push_back(*ipt);

    // Reset ipt to point to correct Ipoint in the vector
    ipt = &ipts.at(index);
  }

  // assign orientation of the dominant response vector
  ipt->orientation = orientation;
}

//-------------------------------------------------------

//! Get the descriptor vector of the provided Ipoint
void Surf::getDescriptor()
{
  int y, x, count=0;
  float scale, *desc, dx, dy, mdx, mdy, co, si;
  float gauss, rx, ry, rrx, rry, sample_x, sample_y, len=0;

  Ipoint *ipt = &ipts.at(index);
  scale = ipt->scale;
  x = cvRound(ipt->x);
  y = cvRound(ipt->y);
  co = cos(ipt->orientation);
  si = sin(ipt->orientation);
  desc = ipt->descriptor;

  // Calculate descriptor for this interest point
  for (int i = -10; i < 10; i+=5)
  {
    for (int j = -10; j < 10; j+=5)
    {
      dx=dy=mdx=mdy=0;
      for (int k = i; k < i + 5; ++k)
      {
        for (int l = j; l < j + 5; ++l)
        {
          // Get coords of sample point on the rotated axis
          sample_x = cvRound(x + (-l*scale*si + k*scale*co));
          sample_y = cvRound(y + ( l*scale*co + k*scale*si));

          // Get the gaussian weighted x and y responses
          gauss = gaussian(k*scale, l*scale, 3.3f*scale);
          rx = gauss * haarX(sample_y, sample_x, 2*scale);
          ry = gauss * haarY(sample_y, sample_x, 2*scale);

          // Get the gaussian weighted x and y responses on rotated axis
          rrx = -rx*si + ry*co;
          rry = rx*co + ry*si;

          dx += rrx;
          dy += rry;
          mdx += fabs(rrx);
          mdy += fabs(rry);
        }
      }

      // add the values to the descriptor vector
      desc[count++] = dx;
      desc[count++] = dy;
      desc[count++] = mdx;
      desc[count++] = mdy;

      // store the current length^2 of the vector
      len += dx*dx + dy*dy + mdx*mdx + mdy*mdy;
    }
  }

  // convert to unit vector
  len = sqrt(len);
  for(int i = 0; i < 64; i++)
    desc[i] /= len;

}

//-------------------------------------------------------

//! Get the upright descriptor vector of the provided Ipoint
void Surf::getUprightDescriptor()
{
  int y, x, count=0;
  float scale, *desc, dx, dy, mdx, mdy;
  float gauss, rx, ry, len = 0.0f;

  Ipoint *ipt = &ipts.at(index);
  scale = ipt->scale;
  y = cvRound(ipt->y);
  x = cvRound(ipt->x);
  desc = ipt->descriptor;

  // Calculate descriptor for this interest point
  for (int i = -10; i < 10; i+=5)
  {
    for (int j = -10; j < 10; j+=5)
    {
      dx=dy=mdx=mdy=0;
      for (int k = i; k < i + 5; ++k)
      {
        for (int l = j; l < j + 5; ++l)
        {
          // get Gaussian weighted x and y responses
          gauss = gaussian(k*scale, l*scale, 3.3f*scale);
          rx = gauss * haarX(k*scale+y, l*scale+x, 2*scale);
          ry = gauss * haarY(k*scale+y, l*scale+x, 2*scale);

          dx += rx;
          dy += ry;
          mdx += fabs(rx);
          mdy += fabs(ry);
        }
      }

      // add the values to the descriptor vector
      desc[count++] = dx;
      desc[count++] = dy;
      desc[count++] = mdx;
      desc[count++] = mdy;

      // store the current length^2 of the vector
      len += dx*dx + dy*dy + mdx*mdx + mdy*mdy;
    }
  }

  // convert to unit vector
  len = sqrt(len);
  for(int i = 0; i < 64; i++)
    desc[i] /= len;

}

//-------------------------------------------------------

//! Calculate the value of the 2d gaussian at x,y
inline float Surf::gaussian(int x, int y, float sig)
{
  return 1.0f/(2.0f*pi*sig*sig) * exp( -(x*x+y*y)/(2.0f*sig*sig));
}

//-------------------------------------------------------

//! Calculate the value of the 2d gaussian at x,y
inline float Surf::gaussian(float x, float y, float sig)
{
  return 1.0f/(2.0f*pi*sig*sig) * exp( -(x*x+y*y)/(2.0f*sig*sig));
}

//-------------------------------------------------------

//! Calculate Haar wavelet responses in x direction
inline float Surf::haarX(int row, int column, int s)
{
  return Area(img, column, row-s/2, s/2, s)
    -1 * Area(img, column-s/2, row-s/2, s/2, s);
}

//-------------------------------------------------------

//! Calculate Haar wavelet responses in y direction
inline float Surf::haarY(int row, int column, int s)
{
  return Area(img, column-s/2, row, s, s/2)
    -1 * Area(img, column-s/2, row-s/2, s, s/2);
}

//-------------------------------------------------------

//! Get the angle from the +ve x-axis of the vector given by (X Y)
float Surf::getAngle(float X, float Y)
{
  if(X >= 0 && Y >= 0)
    return atan(Y/X);

  if(X < 0 && Y >= 0)
    return pi - atan(-Y/X);

  if(X < 0 && Y < 0)
    return pi + atan(Y/X);

  if(X >= 0 && Y < 0)
    return 2*pi - atan(-Y/X);

  return 0;
}
