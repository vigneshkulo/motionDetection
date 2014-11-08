#include<stdio.h>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>

#define BLUR_SIZE	10
#define SEN		160

using namespace cv;
using namespace std;
int main(int argc, char** argv)
{
	Mat image, img;
	Mat img_gray;
	
	vector<vector<Point> > v;

	image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	namedWindow("Image", CV_WINDOW_NORMAL);
	imshow("Image", image);

	cvtColor( image, img_gray, CV_BGR2GRAY);

	threshold (img_gray, img, SEN, 255, THRESH_BINARY);

	blur( img, img, Size(BLUR_SIZE, BLUR_SIZE));

	threshold (img, img, SEN, 255, THRESH_BINARY);
	
	vector<Vec4i> hierarchy;
	findContours (img, v, hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	vector<Rect> boundRect (v.size());

	printf("* Size: %ld\n", v.size());
	vector<vector<Point> > con_poly(v.size());

	Mat drawing = image;//Mat::zeros( img.size(), CV_8UC3 );

	RNG rng(12345);
	#if 1
	for(int i = 0; i < v.size(); i++)
	{
		approxPolyDP ( Mat (v[i]), con_poly[i], 3, true);
		boundRect[i] = boundingRect ( Mat(con_poly[i]) );
	}
	for(int i = 0; i < v.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
		rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
	}
	#endif

	namedWindow("Track", CV_WINDOW_NORMAL);
	imshow("Track", drawing);

	printf("* Width: %d\n", image.size().width);
	printf("* Height: %d\n", image.size().height);
	
	waitKey(0);
	return 0;
}

