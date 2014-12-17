/* based_task.c -- A basic real-time task skeleton. 
 *
 * This (by itself useless) task demos how to setup a 
 * single-threaded LITMUS^RT real-time task.
 */

/* First, we include standard headers.
 * Generally speaking, a LITMUS^RT real-time task can perform any
 * system call, etc., but no real-time guarantees can be made if a
 * system call blocks. To be on the safe side, only use I/O for debugging
 * purposes and from non-real-time sections.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Headers Required by Target Tracking Application */
#include <iostream>
#include <unistd.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

/* Second, we include the LITMUS^RT user space library header.
 * This header, part of liblitmus, provides the user space API of
 * LITMUS^RT.
 */
#include "litmus.h"

/* Next, we define period and execution cost to be constant. 
 * These are only constants for convenience in this example, they can be
 * determined at run time, e.g., from command line parameters.
 *
 * These are in milliseconds.
 */
/* The Input video runs at 30 frames/second => Period = 33.33 */
#define PERIOD            34
#define RELATIVE_DEADLINE 34
#define EXEC_COST         10

/* Catch errors.
 */
#define CALL( exp ) do { \
		int ret; \
		ret = exp; \
		if (ret != 0) \
			fprintf(stderr, "%s failed: %m\n", #exp);\
		else \
			fprintf(stderr, "%s ok.\n", #exp); \
	} while (0)


/* Declare the periodically invoked job. 
 * Returns 1 -> task should exit.
 *         0 -> task should continue.
 */

int job(void);

/* Target Tracking Global Variables */
using namespace cv;
using namespace std;

#define BLUR_SIZE	10
#define SEN		50
#define DE

Mat diffImg, thrImg;
Mat refFrame, refFrameGry;
Mat curFrame, curFrameGry;

char name[25];
int numFrame = 0;
int countFrame = 0;

/* Video file can be replace with direct feed from hardware by just
 * replacing "sample.avi" with the camera device number.
 */
/* Input Video File */
VideoCapture capture("sample.avi");
vector<Vec4i> hierarchy;
vector<vector<Point> > contour;

/* typically, main() does a couple of things: 
 * 	1) parse command line parameters, etc.
 *	2) Setup work environment.
 *	3) Setup real-time parameters.
 *	4) Transition to real-time mode.
 *	5) Invoke periodic or sporadic jobs.
 *	6) Transition to background mode.
 *	7) Clean up and exit.
 *
 * The following main() function provides the basic skeleton of a single-threaded
 * LITMUS^RT real-time task. In a real program, all the return values should be 
 * checked for errors.
 */
int main(int argc, char** argv)
{
	int do_exit;
	struct rt_task param;

	/* Setup task parameters */
	init_rt_task_param(&param);
	param.exec_cost = ms2ns(EXEC_COST);
	param.period = ms2ns(PERIOD);
	param.relative_deadline = ms2ns(RELATIVE_DEADLINE);

	/* What to do in the case of budget overruns? */
	param.budget_policy = NO_ENFORCEMENT;

	/* The task class parameter is ignored by most plugins. */
	param.cls = RT_CLASS_SOFT;

	/* The priority parameter is only used by fixed-priority plugins. */
	param.priority = LITMUS_LOWEST_PRIORITY;

	/* The task is in background mode upon startup. */


	/*****
	 * 1) Command line paramter parsing would be done here.
	 */



	/*****
	 * 2) Work environment (e.g., global data structures, file data, etc.) would
	 *    be setup here.
	 */

	/* Read Reference background image */
	refFrame = imread("ReferenceImage.jpg");		
	if(!capture.isOpened()){
		cout<<"File Not Opened\n";
		return -1;
	}

	printf("* Num of Frames: %lf\n", capture.get(CV_CAP_PROP_FRAME_COUNT));
	printf("* Frames Per Second: %lf\n", capture.get(CV_CAP_PROP_FPS));
	
	numFrame = capture.get(CV_CAP_PROP_FRAME_COUNT);

	/* Convert Reference image from RGB to Gray scale */
	cvtColor( refFrame, refFrameGry, CV_BGR2GRAY);

	/*****
	 * 3) Setup real-time parameters. 
	 *    In this example, we create a sporadic task that does not specify a 
	 *    target partition (and thus is intended to run under global scheduling). 
	 *    If this were to execute under a partitioned scheduler, it would be assigned
	 *    to the first partition (since partitioning is performed offline).
	 */
	CALL( init_litmus() );

//	#define PSNEDF		// Uncomment this to enable Partitioned EDF
	#ifdef PSNEDF
	// Partitioned EDF 
	printf("* Partioned EDF\n");
	param.cpu = 7; // 0, 1, 2, 3, 4, 5, 6, 7
	be_migrate_to_cpu(7); 
	#endif

//	#define CEDF		// Uncomment this to enable Clustered EDF
	#ifdef CEDF
	printf("* Clustered EDF\n");
	param.cpu = cluster_to_first_cpu(0, 4);
	be_migrate_to_cluster(0,4); 
	#endif

	/* To specify a partition, do
	 *
	 * param.cpu = CPU; // 0, 1 ,2 ,3
	 * be_migrate_to_cpu(CPU); 
	 *
	 * where CPU ranges from 0 to "Number of CPUs" - 1 before calling
	 * set_rt_task_param().
	 */
	CALL( set_rt_task_param(gettid(), &param) );

	/*****
	 * 4) Transition to real-time mode.
	 */
	CALL( task_mode(LITMUS_RT_TASK) );

	/* The task is now executing as a real-time task if the call didn't fail. 
	 */

	/*****
	 * 5) Invoke real-time jobs.
	 */
	do {
		/* Wait until the next job is released. */
		sleep_next_period();
		/* Invoke job. */
		do_exit = job();		
	} while (!do_exit);

	/*****
	 * 6) Transition to background mode.
	 */
	CALL( task_mode(BACKGROUND_TASK) );

	/***** 
	 * 7) Clean up, maybe print results and stats, and exit.
	 */
	return 0;
}

/* Replace the code in this function with any other foreground detection algorithm's 
 * like Temporal differencing or Independent Component Analysis etc. 
 * The algorithm we used is Background subtraction
 */
int job(void) 
{
	/* Do real-time calculation. */

	/* Increment the Frame Count */
	countFrame++;

	/* If all frames are processed => EXIT */
	if(countFrame >= numFrame)
	{
		return 1;
	}

	/* Read the Current Frame */
	capture.read(curFrame);

	/* Display the Reference Background Image and the Current Frame */
	#ifdef DE 
	namedWindow("Reference", CV_WINDOW_NORMAL);
	/* Adjust the values to position the window according to Screen dimensions */
	cvMoveWindow("Reference", 200, 50);
	imshow("Reference", refFrame);

	namedWindow("Current", CV_WINDOW_NORMAL);
	cvMoveWindow("Current", 600, 50);
	imshow("Current", curFrame);
	#endif

	/* Convert from RGB to Gray Scale, to work the image */
	cvtColor( curFrame, curFrameGry, CV_BGR2GRAY);

	/* Subtract the Current frame from Reference Background Image */
	absdiff(refFrameGry, curFrameGry, diffImg);

	/* Apply Thresholding to the Difference Image */
	threshold (diffImg, thrImg, SEN, 255, THRESH_BINARY);

	/* Blur the Image to remove noise */
	blur( thrImg, thrImg, Size(BLUR_SIZE, BLUR_SIZE));

	/* Apply Thresholding to the Difference Image */
	threshold (thrImg, thrImg, SEN, 255, THRESH_BINARY);
	
	/* Display the Threshold Image */
	#ifdef DE 
	namedWindow("Threshold", CV_WINDOW_NORMAL);
	cvMoveWindow("Threshold", 200, 400);
	imshow("Threshold", thrImg);
	#endif

	/* Find the contours present in the Thresholded Image */
	findContours (thrImg, contour, hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	vector<Rect> boundRect (contour.size());
	vector<vector<Point> > con_poly(contour.size());

	Mat drawing = curFrame; 

	Scalar color = Scalar(0, 0, 0);

	/* Draw a bound rectangle from the Contour obtained above */
	for(unsigned int i = 0; i < contour.size(); i++)
	{
		approxPolyDP ( Mat (contour[i]), con_poly[i], 3, true);
		boundRect[i] = boundingRect ( Mat(con_poly[i]) );
	}
	for(unsigned int i = 0; i < contour.size(); i++)
	{
		rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
	}

	#ifdef DE 
	namedWindow("Track", CV_WINDOW_NORMAL);
	cvMoveWindow("Track", 600, 400);
	imshow("Track", drawing);
	#endif

	waitKey(20);

	return 0;
}
