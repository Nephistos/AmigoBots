#include <Aria.h>
#include <iostream>
#include <fstream>
#include <ariaUtil.h>
#include <ArTransform.h>

using namespace std;
extern ArRobot robot;

class PrintFiles : public ArASyncTask
{

private:
	double x, y, theta, xt, yt, v = 0, w = 0;
	unsigned long int t;
	ArPose odometrie;

public:

	PrintFiles();
	void* runThread(void*);
};