all:
	g++ motionDetection.cpp `pkg-config opencv --cflags --libs` -o track 
