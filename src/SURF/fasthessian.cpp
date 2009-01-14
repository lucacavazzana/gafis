#include <opencv/cv.h>
#include "integral.h"
#include "fasthessian.h"
#include "ipoint.h"
#include "utils.h"
#include <vector>
#include <iostream>
using namespace std;



//! Destructor
FastHessian::~FastHessian()
{
  // free the det array
  if (m_det) delete [] m_det;
}

//-------------------------------------------------------

//! Constructor without image
FastHessian::FastHessian(std::vector<Ipoint> &ipts,
                         const int octaves, const int intervals, const int init_sample,
                         const float thres)
                         : ipts(ipts), m_det(NULL), i_width(0), i_height(0)
{
  // Initialise variables and construct Det array
  this->octaves = octaves;
  this->intervals = intervals;
  this->init_sample = init_sample;
  this->thres = thres;
}

//-------------------------------------------------------

//! Constructor with image
FastHessian::FastHessian(IplImage *img, std::vector<Ipoint> &ipts,
                         const int octaves, const int intervals, const int init_sample,
                         const float thres)
                         : ipts(ipts), m_det(NULL), i_width(0), i_height(0)
{
  // Initialise variables and construct Det array
  this->octaves = octaves;
  this->intervals = intervals;
  this->init_sample = init_sample;
  this->thres = thres;

  // Set the current image
  setIntImage(img);
}

//-------------------------------------------------------

//! Set or re-set the integral image source
void FastHessian::setIntImage(IplImage *img)
{
  // Change the source image
  this->img = img;

  // Redefine width, height and det map only if image has changed size
  if (img->width != i_width || img->height != i_height)
  {
    i_width = img->width;
    i_height = img->height;

    // Allocate space for determinant of hessian pyramid
    if (m_det) delete [] m_det;
    m_det = new float [octaves*intervals*i_width*i_height];
    memset(m_det,0,sizeof(m_det));
  }
}

//-------------------------------------------------------

//! Find the image features and write into vector of features
void FastHessian::getIpoints()
{
  // Clear the vector of exisiting ipts
  ipts.clear();

  // Calculate approximated determinant of hessian values
  buildDet();

  for(int o=0; o < octaves; o++)
  {
    // for each octave double the sampling step of the previous
    int step = init_sample * cvRound(pow(2.0f,o));

    // determine border width for the largest filter for each ocave
    int border = (3 * cvRound(pow(2.0,o+1)*(intervals)+1) + 1)/2;

    // check for maxima across the scale space
    for(int i = 1; i < intervals-1; ++i)
      for(int r = (border); r < i_height - (border); r += step)
        for(int c = (border); c < i_width - (border); c += step)
          if(isExtremum(o, i, c, r))
            interp_extremum(o,i,r,c);
  }
}

//-------------------------------------------------------

//! Calculate determinant of hessian responses
void FastHessian::buildDet()
{
  int lobe, border, step;
  float Dxx, Dyy, Dxy, scale;

  for(int o=0; o<octaves; o++)
  {
    // calculate filter border for this octave
    border = (3 * fRound(pow(2.0f,o+1)*(intervals)+1) + 1)/2;
    step = init_sample * fRound(pow(2.0f,o));

    for(int i=0; i<intervals; i++) {

      // calculate lobe length (filter side length/3)
      lobe = fRound(pow(2.0f,o+1)*(i+1)+1);
      scale = 1.0f/pow((float)(3*lobe),2);

      for(int r=border; r<i_height-border; r+=step)
      {
        for(int c=border; c<i_width-border; c+=step)
        {
          Dyy = Area(img, c-(lobe-1), r-((3*lobe-1)/2), (2*lobe-1), lobe)
             -2*Area(img, c-(lobe-1), r-((lobe-1)/2), (2*lobe-1), lobe)
               +Area(img, c-(lobe-1), r+((lobe+1)/2), (2*lobe-1), lobe);

          Dxx = Area(img, c-((3*lobe-1)/2), r-(lobe-1), lobe, (2*lobe-1))
             -2*Area(img, c-((lobe-1)/2), r-(lobe-1), lobe, (2*lobe-1))
               +Area(img, c+((lobe+1)/2), r-(lobe-1), lobe, (2*lobe-1));

          Dxy =  Area(img, c-lobe, r-lobe, lobe, lobe)
                +Area(img, c+1, r+1, lobe, lobe)
                -Area(img, c-lobe, r+1, lobe, lobe)
                -Area(img, c+1, r-lobe, lobe, lobe);

          // Normalise the filter responses with respect to their size
          Dxx *= scale;
          Dyy *= scale;
          Dxy *= scale;

          // Get the sign of the laplacian
          int lap_sign = (Dxx+Dyy >= 0 ? 1 : -1);

          // Get the determinant of hessian response
          float res = (Dxx*Dyy - pow(0.9f*Dxy,2));
          res = (res < thres ? 0 : lap_sign * res);

          // calculate approximated determinant of hessian value
          m_det[(o*intervals+i)*(i_width*i_height) + (r*i_width+c)] = res;
        }
      }
    }
  }
}

//-------------------------------------------------------

//! Non Maximal Suppression function
int FastHessian::isExtremum(int octave, int interval, int c, int r)
{
  float val = getVal(octave, interval, c, r);
  int step = init_sample * cvRound(pow(2.0f,octave));

  // reject points with low response to the det of hessian function
  if(val < thres) return 0;

  // check for maximum
  for(int i = -1; i <= 1; i++ )
    for(int j = -step; j <= step; j+=step )
      for(int k = -step; k <= step; k+=step )
        if (i != 0 || j != 0 || k != 0)
          if(getVal(octave, interval+i, c+j, r+k) > val)
            return 0;

  return 1;
}

//-------------------------------------------------------

//! Return the value of the approximated determinant of hessian
inline float FastHessian::getVal(int o, int i, int c, int r)
{
  return fabs(m_det[(o*intervals+i)*(i_width*i_height) + (r*i_width+c)]);
}

//-------------------------------------------------------

//! Return the sign of the laplacian (trace of the hessian)
inline int FastHessian::getLaplacian(int o, int i, int c, int r)
{
  float res = (m_det[(o*intervals+i)*(i_width*i_height) + (r*i_width+c)]);

  return (res >= 0 ? 1 : -1);
}

//-------------------------------------------------------

//! Round an int to a float
inline int FastHessian::fRound(float flt)
{
  return (int) floor(flt+0.5f);
}

//-------------------------------------------------------

//! Return the value of the approximated determinant of hessian
inline float FastHessian::getValLowe(int o, int i, int r, int c)
{
  return fabs(m_det[(o*intervals+i)*(i_width*i_height) + (r*i_width+c)]);
}

//-------------------------------------------------------

//Interpolates a scale-space extremum's location and scale to subpixel
//accuracy to form an image feature.
void FastHessian::interp_extremum(int octv, int intvl, int r, int c)
{
	double xi = 0, xr = 0, xc = 0;
	int i = 0;
  int step = init_sample * cvRound(pow(2.0f,octv));

  // Get the offsets to the actual location of the extremum
  interp_step( octv, intvl, r, c, &xi, &xr, &xc );

  // If point is sufficiently close to the actual extremum
  if( fabs( xi ) <= 0.5  &&  fabs( xr ) <= 0.5  &&  fabs( xc ) <= 0.5 )
  {
    // Create Ipoint and push onto Ipoints vector
    Ipoint ipt;
    ipt.x = (float) (c + step*xc);
    ipt.y = (float) (r + step*xr);
    ipt.scale = (float) ((1.2f/9.0f) * (3*(pow(2.0f, octv+1) * (intvl+xi+1)+1)));
    ipt.laplacian = getLaplacian(octv, intvl, c, r);
    ipts.push_back(ipt);
  }
}

//-------------------------------------------------------

//Performs one step of extremum interpolation.
void FastHessian::interp_step( int octv, int intvl, int r, int c, double* xi, double* xr, double* xc )
{
	CvMat* dD, * H, * H_inv, X;
	double x[3] = { 0 };

	dD = deriv_3D( octv, intvl, r, c );
	H = hessian_3D( octv, intvl, r, c );
	H_inv = cvCreateMat( 3, 3, CV_64FC1 );
	cvInvert( H, H_inv, CV_SVD );
	cvInitMatHeader( &X, 3, 1, CV_64FC1, x, CV_AUTOSTEP );
	cvGEMM( H_inv, dD, -1, NULL, 0, &X, 0 );

	cvReleaseMat( &dD );
	cvReleaseMat( &H );
	cvReleaseMat( &H_inv );

	*xi = x[2];
	*xr = x[1];
	*xc = x[0];
}

//-------------------------------------------------------

//Computes the partial derivatives in x, y, and scale of a pixel in the DoG
//scale space pyramid.
CvMat* FastHessian::deriv_3D( int octv, int intvl, int r, int c )
{
	CvMat* dI;
	double dx, dy, ds;
  int step = init_sample * cvRound(pow(2.0f,octv));

  dx = ( getValLowe(octv,intvl, r, c+step ) -
		getValLowe( octv,intvl, r, c-step ) ) / 2.0;
	dy = ( getValLowe( octv,intvl, r+step, c ) -
		getValLowe( octv,intvl, r-step, c ) ) / 2.0;
	ds = ( getValLowe( octv,intvl+1, r, c ) -
		getValLowe( octv,intvl-1, r, c ) ) / 2.0;

	dI = cvCreateMat( 3, 1, CV_64FC1 );
	cvmSet( dI, 0, 0, dx );
	cvmSet( dI, 1, 0, dy );
	cvmSet( dI, 2, 0, ds );

	return dI;
}

//-------------------------------------------------------

//Computes the 3D Hessian matrix for a pixel in the DoG scale space pyramid.
CvMat* FastHessian::hessian_3D(int octv, int intvl, int r, int c )
{
	CvMat* H;
	double v, dxx, dyy, dss, dxy, dxs, dys;
  int step = init_sample * cvRound(pow(2.0f,octv));

	v = getValLowe( octv,intvl, r, c );
	dxx = ( getValLowe( octv,intvl, r, c+step ) +
			getValLowe( octv,intvl, r, c-step ) - 2 * v );
	dyy = ( getValLowe( octv,intvl, r+step, c ) +
			getValLowe( octv,intvl, r-step, c ) - 2 * v );
	dss = ( getValLowe( octv,intvl+1, r, c ) +
			getValLowe( octv,intvl-1, r, c ) - 2 * v );
	dxy = ( getValLowe( octv,intvl, r+step, c+step ) -
			getValLowe( octv,intvl, r+step, c-step ) -
			getValLowe( octv,intvl, r-step, c+step ) +
			getValLowe( octv,intvl, r-step, c-step ) ) / 4.0;
	dxs = ( getValLowe( octv,intvl+1, r, c+step ) -
			getValLowe( octv,intvl+1, r, c-step ) -
			getValLowe( octv,intvl-1, r, c+step ) +
			getValLowe( octv,intvl-1, r, c-step ) ) / 4.0;
	dys = ( getValLowe( octv,intvl+1, r+step, c ) -
			getValLowe( octv,intvl+1, r-step, c ) -
			getValLowe( octv,intvl-1, r+step, c ) +
			getValLowe( octv,intvl-1, r-step, c ) ) / 4.0;

	H = cvCreateMat( 3, 3, CV_64FC1 );
	cvmSet( H, 0, 0, dxx );
	cvmSet( H, 0, 1, dxy );
	cvmSet( H, 0, 2, dxs );
	cvmSet( H, 1, 0, dxy );
	cvmSet( H, 1, 1, dyy );
	cvmSet( H, 1, 2, dys );
	cvmSet( H, 2, 0, dxs );
	cvmSet( H, 2, 1, dys );
	cvmSet( H, 2, 2, dss );

	return H;
}

//-------------------------------------------------------
