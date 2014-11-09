#include<stdio.h>
#include<unistd.h>
#include<iostream>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>

#define BLUR_SIZE	10
#define SEN		50

#define DE

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
	printf("* FPS: %lf\n", capture.get(CV_CAP_PROP_FPS));
	printf("* Frame#: %lf\n", capture.get(CV_CAP_PROP_POS_FRAMES));
	
	char name[25];
	int numFrame = capture.get(CV_CAP_PROP_FRAME_COUNT);
	vector<Vec4i> hierarchy;
	vector<vector<Point> > contour;

//	numFrame = 1;
	cvtColor( refFrame, refFrameGry, CV_BGR2GRAY);

	for(int i = 0; i < numFrame; i++)
	{
		capture.read(curFrame);

		#if 0
                sprintf(name, "images/frame%lf.jpg", capture.get(CV_CAP_PROP_POS_FRAMES));
                imwrite(name, curFrame);
		#endif

		#ifdef DE 
		namedWindow("Reference", CV_WINDOW_NORMAL);
		imshow("Reference", refFrame);

		namedWindow("Current", CV_WINDOW_NORMAL);
		imshow("Current", curFrame);
		#endif

		cvtColor( curFrame, curFrameGry, CV_BGR2GRAY);

		absdiff(refFrameGry, curFrameGry, diffImg);
	
		#ifdef DE 
		namedWindow("Diff", CV_WINDOW_NORMAL);
		imshow("Diff", diffImg);
		#endif

		threshold (diffImg, thrImg, SEN, 255, THRESH_BINARY);

		blur( thrImg, thrImg, Size(BLUR_SIZE, BLUR_SIZE));

		threshold (thrImg, thrImg, SEN, 255, THRESH_BINARY);
		
		#ifdef DE 
		namedWindow("Threshold", CV_WINDOW_NORMAL);
		imshow("Threshold", thrImg);
		#endif

		findContours (thrImg, contour, hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		vector<Rect> boundRect (contour.size());
		vector<vector<Point> > con_poly(contour.size());

		Mat drawing = curFrame; //Mat::zeros( thrImg.size(), CV_8UC3 );

		RNG rng(12345);

		#if 1
		for(int i = 0; i < contour.size(); i++)
		{
			approxPolyDP ( Mat (contour[i]), con_poly[i], 3, true);
			boundRect[i] = boundingRect ( Mat(con_poly[i]) );
		}
		for(int i = 0; i < contour.size(); i++)
		{
			Scalar color = Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
			rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		}
		#endif

		#ifdef DE 
		namedWindow("Track", CV_WINDOW_NORMAL);
		imshow("Track", drawing);
		#endif

		waitKey(20);
	}
	waitKey(0);
	return 0;
}

