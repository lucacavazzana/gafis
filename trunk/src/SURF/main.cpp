#include "surflib.h"

//-------------------------------------------------------
// Define PROCEDURE as:
//  - 1 and supply image path to run on static image
//  - 2 to capture from a webcam
#define PROCEDURE 1

//-------------------------------------------------------

int mainImage2(const char *filename);
int mainVideo(void);

int mainSURF()
{
  if (PROCEDURE == 1) return mainImage2("test.png");
  if (PROCEDURE == 2) return mainVideo();
}

//-------------------------------------------------------

int mainImage2(const char *filename)
{
  // Declare Ipoints and other stuff
  IpVec ipts;
  IplImage *img=cvLoadImage(filename);

  // Detect and describe interest points in the image
  surfDetDes(img, ipts, false, 3, 4, 1, 0.0004f);

  // Draw the detected points
  drawIpoints(img, ipts);

  // Display the result
  showImage(img);

  return 0;
}

//-------------------------------------------------------

int mainVideo2(void)
{
  // Initialise capture device
  CvCapture* capture = cvCaptureFromCAM( CV_CAP_ANY );
  if(!capture) error("No Capture");

  // Create a window
  cvNamedWindow("OpenSURF", CV_WINDOW_AUTOSIZE );

  // Declare Ipoints and other stuff
  IpVec ipts;
  IplImage *img=NULL;

  // Main capture loop
  while( 1 )
  {
    // Grab frame from the capture source
    img = cvQueryFrame(capture);

    // Detect and describe interest points in the image
    surfDetDes(img, ipts, false, 3, 4, 2, 0.002f);

    // Draw the detected points
    drawIpoints(img, ipts);

    // Draw the FPS figure
    drawFPS(img);

    // Display the result
    cvShowImage("OpenSURF", img);

    // If ESC key pressed exit loop
    if( (cvWaitKey(10) & 255) == 27 ) break;
  }

  // Release the capture device
  cvReleaseCapture( &capture );
  cvDestroyWindow( "OpenSURF" );
  return 0;
}

//-------------------------------------------------------
