#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "integral.h"
#include "utils.h"


//! Computes the integral image of image img.  Assumes source image to be a
//! 32-bit floating point.  Returns IplImage of 32-bit float form.
IplImage *Integral(IplImage *source)
{
  // convert the image to single channel 32f
  IplImage *img = getGray(source);
  IplImage *int_img = cvCreateImage(cvGetSize(img), IPL_DEPTH_32F, 1);

  // set up variables for data access
  int height = img->height;
  int width = img->width;
  int step = img->widthStep/sizeof(float);
  float *data   = (float *) img->imageData;
  float *i_data = (float *) int_img->imageData;

  // calculate integral image values
  for(int i=0; i<height; i++)
  {
    float rs = 0.0f;
    for(int j=0; j<width; j++)
    {
      // cumulative row sum
      rs = rs+data[i*step+j];

      // integral image sum
      if (i*step+j >= step)
      {
        i_data[i*step+j] = i_data[(i-1)*step+j]+rs;
      }
      else
      {
        // these values computed for first row only
        i_data[i*step+j] = rs;
      }
    }
  }

  // release the gray image
  cvReleaseImage(&img);

  // return the integral image
  return int_img;
}

//-------------------------------------------------------

//! Computes the sum of pixels within the rectangle specified by the top-left start
//! co-ordinate (x,y) with width w and height h.
float Area(IplImage *img, int x, int y, int w, int h)
{
  float *data = (float *) img->imageData;
  int step = img->widthStep/sizeof(float);
  int x1=x-1, y1=y-1, x2=x1+w, y2=y1+h;

  // if all values are within bounds return BR + TL - (BL + TR)
  if (x1>=0 && y1 >=0 && x2<img->width && y2<img->height)
  {
    return data[y2*step+x2] + data[y1*step+x1]
          -(data[y2*step+x1] + data[y1*step+x2]);
  }
  else
    return 0;
}
