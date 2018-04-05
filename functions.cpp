#include "functions.h"

void waitUntilGoalAchieved(ArRobot &robot, ArActionGoto &gotoPoseAction)
{
	while (Aria::getRunning())
	{
		robot.lock();
		if (gotoPoseAction.haveAchievedGoal())
		{
			gotoPoseAction.cancelGoal();
			robot.unlock();
			ArUtil::sleep(1);
			break;
		}
		robot.unlock();
		ArUtil::sleep(1);
	}
	gotoPoseAction.cancelGoal();
}

void changeHeading(ArRobot &robot, double HeadingAngle)
{
	robot.setHeading(HeadingAngle);
	while (!robot.isHeadingDone())
	{
		ArUtil::sleep(1);
	}
	robot.lock();
	robot.clearDirectMotion();
	robot.setLatVelMax(200);
	robot.unlock();
	ArUtil::sleep(1);
}

void goStraightX(ArRobot &robot, double x)
{
	double k = 0.9, error = 5, v = 0;
	while (abs(robot.getX() - x) > error)
	{


		if (abs(robot.getTh()) < 90)
			v = k * (x - robot.getX());
		else
			v = -k * (x - robot.getX());

		robot.lock();
		robot.setVel(v);
		robot.unlock();
		ArUtil::sleep(1);
	}
	robot.lock();
	robot.setVel(0);
	robot.clearDirectMotion();
	robot.setLatVelMax(200);
	robot.unlock();
	ArUtil::sleep(1);
}