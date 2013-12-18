// Luke Mitchell, 2013
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

void pixelate(Mat &frame, int x, int y, int x2, int y2, int blksize = 15);
void __pixelate_uchar(Mat &frame, int x, int y, int x2, int y2, int blksize);

int main( int argc, const char** argv )
{
	VideoCapture cap;
	
	if(argc > 1)
	{
		cap.open(string(argv[1]));
	}
	else
	{
		cap.open(CV_CAP_ANY);
	}
	
	if(!cap.isOpened())
	{
		printf("Error: could not load a camera or video.\n");
	}
	
	// Matrices
	Mat frame, frame_gray, frame_pixelated;
	
	// Create window
	namedWindow("Pixelate", 1);
	
	while (1) {
	  waitKey(20);
	  cap >> frame;
	  
	  if(!frame.data)
		{
			printf("Error: no frame data from camera\n");
			break;
		}
		
		// flip for intuition
		flip(frame, frame, 1);
		
		// gray it!
	  cvtColor(frame, frame_gray, CV_BGR2GRAY);

    // pixelate!
    pixelate(frame_gray, 0, 0, frame.cols, frame.rows);
    
    // display!
	  imshow("Pixelate", frame_gray);
	}
	
	return EXIT_SUCCESS;
}

void pixelate(Mat &frame, int x, int y, int x2, int y2, int blksz)
{  
  // get type
  uchar type = frame.type() & CV_MAT_DEPTH_MASK;
  uchar chans = frame.channels();

  // process accordingly
  if (type == CV_8U && chans == 1) return __pixelate_uchar(frame, x, y, x2, y2, blksz);
  
  // or print an error
  cerr << "Could not pixellate image type." << endl;
}

void __pixelate_uchar(Mat &frame, int x, int y, int x2, int y2, int blksz)
{  
  // counters
  float sum;
  int avg, count;
  
  // loop invariants
  int d = blksz / 2;
  int blksz_squared = blksz * blksz;
  
  // get the data pointer
  uchar* frame_data = frame.data;
  
  // process
  for (int i = y; i < y2; i += blksz)
  {    
    for (int j = x; j < x2; j += blksz)
    {
      sum = 0;
      count = 0;
      
      // sum pixels in neighbourhood
      for (int r = 0; r < blksz; r++)
      {
        int row = frame.cols * (i + r);
        for (int c = 0; c < blksz; c++)
        {
          if (i < frame.rows && (j + c) < frame.cols)
          {
            sum += frame_data[row + (j + c)];
            count++;
          }
        }
      }
      
      // make an average
      avg = (int) (sum / (float) (count));
      
      // ...and colour each pixel in that neighbourhood that value
      for (int r = 0; r < blksz; r++)
      {
        int row = frame.cols * (i + r);
        for (int c = 0; c < blksz; c++)
        {
          if (i < frame.rows && (j + c) < frame.cols)
          {
            frame_data[row + (j + c)] = avg;
          }
        }
      }
    }
  }
}
