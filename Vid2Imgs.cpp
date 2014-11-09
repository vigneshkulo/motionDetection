#include<stdio.h>
#include<iostream>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	Mat image;
		
        VideoCapture capture("sample.avi");
        if(!capture.isOpened())
	{
                cout<<"File Not Opened\n";
                return -1;
        }

	printf("* Num of Frames: %lf\n", capture.get(CV_CAP_PROP_FRAME_COUNT));
	
	char name[25];
	for(int i = 0; i < capture.get(CV_CAP_PROP_FRAME_COUNT); i++)
	{
		capture.read(image);
		sprintf(name, "images/frame%lf.jpg", capture.get(CV_CAP_PROP_POS_FRAMES));
		imwrite(name, image);
	}

	waitKey(0);
	return 0;
}

