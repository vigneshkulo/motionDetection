#include<time.h>
#include<stdio.h>
#include<unistd.h>
#include<iostream>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>

#define BLUR_SIZE	10
#define SEN		50

#define DE
//#define LOG_RESP

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	Mat diffImg, thrImg;
	Mat refFrame, refFrameGry;
	Mat curFrame, curFrameGry;

	refFrame = imread("ReferenceImage.jpg");		
	VideoCapture capture("sample.avi");
	if(!capture.isOpened()){
		cout<<"File Not Opened\n";
		return -1;
	}

	printf("* Num of Frames: %lf\n", capture.get(CV_CAP_PROP_FRAME_COUNT));
	
	char name[25];
	int numFrame = capture.get(CV_CAP_PROP_FRAME_COUNT);
	vector<Vec4i> hierarchy;
	vector<vector<Point> > contour;

	cvtColor( refFrame, refFrameGry, CV_BGR2GRAY);

	#ifdef LOG_RESP 
	char fileName[20];
	struct timespec tpStart;
	struct timespec tpEnd;
	sprintf(fileName, "Resp%s", argv[1]);
	FILE* fpOut = fopen( fileName, "w+");
	#endif

	for(int i = 0; i < numFrame; i++)
	{
		#ifdef LOG_RESP 
		clock_gettime (CLOCK_REALTIME, &tpStart);
		#endif

		capture.read(curFrame);

		#ifdef DE 
		namedWindow("Reference", CV_WINDOW_NORMAL);
		cvMoveWindow("Reference", 200, 50);
		imshow("Reference", refFrame);

		namedWindow("Current", CV_WINDOW_NORMAL);
		cvMoveWindow("Current", 600, 50);
		imshow("Current", curFrame);
		#endif

		cvtColor( curFrame, curFrameGry, CV_BGR2GRAY);

		absdiff(refFrameGry, curFrameGry, diffImg);
	
		threshold (diffImg, thrImg, SEN, 255, THRESH_BINARY);

		blur( thrImg, thrImg, Size(BLUR_SIZE, BLUR_SIZE));

		threshold (thrImg, thrImg, SEN, 255, THRESH_BINARY);
		
		#ifdef DE 
		namedWindow("Threshold", CV_WINDOW_NORMAL);
		cvMoveWindow("Threshold", 200, 400);
		imshow("Threshold", thrImg);
		#endif

		findContours (thrImg, contour, hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		vector<Rect> boundRect (contour.size());
		vector<vector<Point> > con_poly(contour.size());

		Mat drawing = curFrame; 

		Scalar color = Scalar(0, 0, 0);

		for(int i = 0; i < contour.size(); i++)
		{
			approxPolyDP ( Mat (contour[i]), con_poly[i], 3, true);
			boundRect[i] = boundingRect ( Mat(con_poly[i]) );
		}
		for(int i = 0; i < contour.size(); i++)
		{
			rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		}

		#ifdef DE 
		namedWindow("Track", CV_WINDOW_NORMAL);
		cvMoveWindow("Track", 600, 400);
		imshow("Track", drawing);
		#endif

		waitKey(20);

		#ifdef LOG_RESP 
		clock_gettime (CLOCK_REALTIME, &tpEnd);
		fprintf(fpOut, "%ld, %ld\n", tpEnd.tv_sec - tpStart.tv_sec, tpEnd.tv_nsec - tpStart.tv_nsec);
		#endif
	}
	return 0;
}


