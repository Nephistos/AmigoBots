#include "PrintFiles.h"
#include "functions.h"
#include "Communication.h"
//#include "MAJPos.h"

using namespace std;

ArRobot robot;
Communication communication;


int main(int argc, char **argv)
{

	//*********************************
	//INIT. ESSENTIEL
	printf("Robot definition \n");
	ArArgumentParser parser(&argc, argv);
	parser.loadDefaultArguments();

	ArRobotConnector robotConnector(&parser, &robot);

	if (!robotConnector.connectRobot())
	{
		if (!parser.checkHelpAndWarnUnparsed())
		{
			ArLog::log(ArLog::Terse, "Could not connect to robot");
		}
		else
		{
			ArLog::log(ArLog::Terse, "Error, could not connect to robot.");
			Aria::logOptions();
			Aria::exit(1);
		}
	}

	if (!robot.isConnected())
	{
		ArLog::log(ArLog::Terse, "Internal error: robot connector succeeded but ArRobot::isConnected() is false!");
	}

	if (!Aria::parseArgs() || !parser.checkHelpAndWarnUnparsed())
	{
		Aria::logOptions();
		Aria::exit(1);
		return 1;
	}

	//FIN INIT.
	//*********************************

	//Variables
	//*********************************
	PrintFiles printFiles;
	//MAJPos majpos;

	Aria::init();
	parser.loadDefaultArguments();
	ArSimpleConnector simpleConnector(&parser);

	printFiles.runAsync();
	communication.runAsync();
	//majpos.runAsync();

	double xf, theta, waitTime = 4000;
	//bool load définit le mode de fonctionnement de l'exe généré
	bool simul = false, load = false;
	string staubli_message;

	//Initialisation conditionnelle (charge || décharge)
	ArPose initPose, LoadPos, intermediatPose;
	if (load)
	{
		xf = 3550;
		theta = -90;
		initPose = ArPose(500, 2600);
		LoadPos = ArPose(1000, 2000);
		intermediatPose = ArPose(3550 - 500, 2600);
		staubli_message = "Staubli:ack_charge";
	}
	else
	{
		xf = 3400;
		theta = 90;
		initPose = ArPose(500, 400);
		LoadPos = ArPose(1000, 1000);
		intermediatPose = ArPose(3400 - 500, 800);
		staubli_message = "Staubli:ack_decharge";
	}

	ArTime start;

	// Sonars
	ArSonarDevice sonar;
	ArActionAvoidFront avoidF;
	ArActionAvoidSide avoidS;
	robot.addRangeDevice(&sonar);


	// Make a key handler, so that escape will shut down the program
	// cleanly
	ArKeyHandler keyHandler;
	Aria::setKeyHandler(&keyHandler);
	robot.attachKeyHandler(&keyHandler);
	printf("You may press escape to exit\n");


	// Collision avoidance actions at higher priority
	/*ArActionLimiterForwards limiterAction("speed limiter near", 300, 600, 250);
	robot.addAction(&limiterAction, 26);*/
	robot.setLatVelMax(200);

	// Goto action at lower priority
	ArActionGoto gotoPoseAction("goto");
	robot.addAction(&gotoPoseAction, 25);
	gotoPoseAction.cancelGoal();
	gotoPoseAction.setCloseDist(100);

	// Stop action at lower priority, so the robot stops if it has no goal
	ArActionStop stopAction("stop");
	robot.addAction(&stopAction, 20);

	//Thread en asynchrone
	robot.runAsync(true);

	// turn on the motors, turn off amigobot sounds
	robot.enableMotors();
	robot.comInt(ArCommands::ENABLE, 1);
	robot.comInt(ArCommands::SONAR, 1);
	robot.comInt(ArCommands::SOUNDTOG, 0);

	// add a set of actions that combine together to effect the wander behavior
	ArActionStallRecover recover;
	ArActionAvoidFront avoidFrontNear("Avoid Front Near", 200, 0, -15);
	ArActionAvoidFront avoidFrontFar("Avoid Front Far", 450, 100, -15);

	robot.addAction(&recover, 100);
	robot.addAction(&avoidFrontNear, 50);
	robot.addAction(&avoidFrontFar, 49);

	ArLog::log(ArLog::Normal, "Beging.");

	//Fin Variables
	//*********************************

	//(DE)CHARGEMENT DU CONVOYEUR
	//*********************************

	//Pose initiale du robot selon le mode de fonctionnement
	robot.lock();
	robot.moveTo(initPose);
	robot.unlock();

	while (true)
	{
		while (!communication.getStop())
		{
			cout << "Début" << endl;

			//On rejoint la LoadPos intermédiaire, on recul jusqu'à la LoadPos finale, on avance jusqu'à la LoadPos intermédiaire
			if (load)
			{
				//Maj de la pos. de charge
				if (communication.getBoolPosC())
				{
					LoadPos = communication.getPosC();
				}

				robot.lock();
				gotoPoseAction.setGoal(LoadPos);
				robot.unlock();
				waitUntilGoalAchieved(robot, gotoPoseAction);

				//DESACTIVES SONARS
				avoidFrontFar.deactivate();
				avoidFrontNear.deactivate();

				changeHeading(robot, 0);

				goStraightX(robot, 800);
				ArUtil::sleep(waitTime);

				goStraightX(robot, LoadPos.getX());
				changeHeading(robot, 0);

				//ACTIVES SONARS
				avoidFrontFar.activate();
				avoidFrontNear.activate();

				//On attend l'ordre de charge du convoyeur
				while (!communication.getCharge())
				{
					if (simul)
						break;
					ArUtil::sleep(1);

				}
			}

			//On attend l'ordre de décharge du convoyeur (sauf si l'on est en charge)

			while (!communication.getDecharge())
			{
				if (simul || load)
					break;
				ArUtil::sleep(1);

			}

			//On va à la pos. charge/décharge intermédiaire
			robot.lock();
			gotoPoseAction.setGoal(intermediatPose);
			robot.unlock();

			waitUntilGoalAchieved(robot, gotoPoseAction);

			//DESACTIVES SONARS
			avoidFrontFar.deactivate();
			avoidFrontNear.deactivate();

			//On va à la pos. charge/décharge finale
			changeHeading(robot, 0);
			goStraightX(robot, xf);

			//On envoi l'ack de charge/décharge
			communication.setMessage(staubli_message);
			if (!load)
				waitTime = 20000;
			ArUtil::sleep(waitTime);
			waitTime = 4000;

			//On va à la pos. charge/décharge intermédiaire
			goStraightX(robot, xf - 500);
			changeHeading(robot, theta);

			//ACTIVE SONARS
			avoidFrontFar.activate();
			avoidFrontNear.activate();

			//Maj de la pos. de décharge
			if (!load && communication.getBoolPosD())
				LoadPos = communication.getPosD();

			robot.lock();
			gotoPoseAction.setGoal(LoadPos);
			robot.unlock();

			waitUntilGoalAchieved(robot, gotoPoseAction);

			changeHeading(robot, 0);

			if (!load)
			{

				robot.lock();
				gotoPoseAction.setGoal(LoadPos);
				robot.unlock();
				waitUntilGoalAchieved(robot, gotoPoseAction);

				//DESACTIVES SONARS
				avoidFrontFar.deactivate();
				avoidFrontNear.deactivate();

				changeHeading(robot, 0);

				goStraightX(robot, 800);
				ArUtil::sleep(waitTime);

				goStraightX(robot, LoadPos.getX());
				changeHeading(robot, 0);

				//ACTIVES SONARS
				avoidFrontFar.activate();
				avoidFrontNear.activate();

			}

		}
	}

	//FIN (DE)CHARGEMENT DU CONVOYEUR
	//*********************************

	// Robot disconnected or time elapsed, shut down
	Aria::shutdown();
	return 0;
}