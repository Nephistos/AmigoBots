#include <Aria.h>
#include <iostream>
#include <fstream>
#include <ariaUtil.h>
#include <ArTransform.h>

void waitUntilGoalAchieved(ArRobot &robot, ArActionGoto &gotoPoseAction);
void changeHeading(ArRobot &robot, double HeadingAngle);
void goStraightX(ArRobot &robot, double x);