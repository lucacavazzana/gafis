#ifndef FASTHESSIAN_H
#define FASTHESSIAN_H

#include <opencv/cv.h>
#include "integral.h"
#include "ipoint.h"
#include <vector>

static const int OCTAVES = 3;
static const int INTERVALS = 4;
static const float THRES = 0.002f;
static const int INIT_SAMPLE = 2;


class FastHessian {

  public:

    //! Destructor
    ~FastHessian();

    //! Constructor without image
    FastHessian(std::vector<Ipoint> &ipts,
                const int octaves = OCTAVES,
                const int intervals = INTERVALS,
                const int init_sample = INIT_SAMPLE,
                const float thres = THRES);

    //! Constructor with image
    FastHessian(IplImage *img,
                std::vector<Ipoint> &ipts, int octaves = OCTAVES,
                const int intervals = INTERVALS,
                const int init_sample = INIT_SAMPLE,
                const float thres = THRES);

    //! Set or re-set the integral image source
    void setIntImage(IplImage *img);

    //! Find the image features and write into vector of features
    void getIpoints();

  private:

    //---------------- Private Functions -----------------//

    //! Calculate determinant of hessian responses
    void buildDet();

    //! Non Maximal Suppression function
    int isExtremum(int octave, int interval, int column, int row);

    //! Return the value of the approximated determinant of hessian
    inline float getVal(int octave, int interval, int column, int row);

    //! Return the sign of the laplacian (trace of the hessian)
    inline int getLaplacian(int o, int i, int c, int r);

    //! Round an int to a float
    inline int fRound(float flt);

    //! Interpolation functions - adapted from Lowe's SIFT interpolation
    void interp_extremum(int octv, int intvl, int r, int c);
    void interp_step( int octv, int intvl, int r, int c, double* xi, double* xr, double* xc );
    CvMat* deriv_3D( int octv, int intvl, int r, int c );
    CvMat* hessian_3D(int octv, int intvl, int r, int c );
    inline float getValLowe(int octave, int interval, int column, int row);

    //---------------- Private Variables -----------------//

    //! Pointer to the integral Image, and its attributes
    IplImage *img;
    int i_width, i_height;

    //! Reference to vector of features passed from outside
    std::vector<Ipoint> &ipts;

    //! Number of Octaves
    int octaves;

    //! Number of Intervals per octave
    int intervals;

    //! Initial sampling step for Ipoint detection
    int init_sample;

    //! Threshold value for blob resonses
    float thres;

    //! Array stack of determinant of hessian values
    float *m_det;
};



#endif
