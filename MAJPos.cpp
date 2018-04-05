#include "MAJPos.h"

/* Construtor. Initialize counter. */
MAJPos::MAJPos()
{

}

void* MAJPos::runThread(void*)
{
	while (!communication.getStop())
	{
		while (!communication.getBoolPosR())
		{
			ArUtil::sleep(1);
		}
		robot.moveTo(communication.getPosR());
	}
	ArLog::log(ArLog::Normal, "MAJPos thread: requested stop running, ending thread.");
	return NULL;
}