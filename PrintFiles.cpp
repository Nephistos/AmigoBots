#include "PrintFiles.h"

	/* Construtor. Initialize counter. */
PrintFiles::PrintFiles()
	{

	}

void* PrintFiles::runThread(void*)
	{
		ArTime time;
		ArSensorReading *sensorReading;
		time.setToNow();
		int count = 0;
		// Run until the thread is requested to end by another thread.
		while (this->getRunningWithLock())
		{

			odometrie = robot.getRawEncoderPose();
			t = time.mSecSince();
			x = odometrie.getX();
			y = odometrie.getY();
			theta = odometrie.getTh();
			v = robot.getVel();
			w = robot.getRotVel();


			//cout << "X : " << x << " Y : " << y << " theta : " << theta << endl;
			//ODOMETRIE
			ofstream fichier("odometrie.txt", std::ios_base::app);

			if ((fichier) && (count != 0))
			{
				fichier << x << "\t" << y << "\t" << theta << "\t" << v << "\t" << w << endl;
				fichier.close();
			}

			//SONARS
			for (int i = 0; i < 8; i++)
			{
				sensorReading = robot.getSonarReading(i);

				ArTransform transform(sensorReading->getPose());

				xt = transform.getX();
				yt = transform.getY();

				ofstream fichier2("sonar.txt", std::ios_base::app);

				if ((fichier2) && (count != 0))
				{
					fichier2 << xt << " " << yt << " ";
					fichier2.close();
				}

			}

			ofstream fichier2("sonar.txt", std::ios_base::app);
			fichier2 << endl;
			fichier2.close();


			count++;
			ArUtil::sleep(10);
		}


		ArLog::log(ArLog::Normal, "Print files thread: requested stop running, ending thread.");
		return NULL;
	}
