#include <Aria.h>
#include <iostream>
#include <fstream>
#include <ariaUtil.h>
#include <ArTransform.h>

#ifndef COM_DEF
#define COM_DEF


using namespace std;
extern ArRobot robot;

class Communication : public ArASyncTask
{

private:

	ArMutex myMutex;

	ArPose Ar_posR = ArPose(-1, -1, 0);
	ArPose Ar_posC = ArPose(-1, -1, 0);
	ArPose Ar_posD = ArPose(-1, -1, 0);

	bool charge;
	bool decharge;
	bool ack_charge;
	bool ack_decharge;

	bool bool_posC = false;
	bool bool_posD = false;
	bool bool_posR = false;

	bool stop = true;

	std::string str_message;

	ArPose getPoseFromStr(std::string str_pos);

public:

	Communication();
	void* runThread(void*);

	//Lorsque Victor demande de lui livrer un palet

	bool getCharge();
	bool getDecharge();
	/*
	bool setAckCharge();
	bool setAckDecharge();
	*/


	ArPose getPosC();
	ArPose getPosD();
	ArPose getPosR();

	bool getBoolPosC();
	bool getBoolPosD();
	bool getBoolPosR();
	bool getStop();

	void setMessage(std::string message);
};

#endif 