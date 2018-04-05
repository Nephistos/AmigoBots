#include <Aria.h>
#include <iostream>
#include <fstream>
#include <ariaUtil.h>
#include <ArTransform.h>
#include "Communication.h"


using namespace std;
extern ArRobot robot;
extern Communication communication;

class MAJPos : public ArASyncTask
{

private:

public:

	MAJPos();
	void* runThread(void*);
};