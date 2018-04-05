#include "Communication.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include "src\TCP\Utils\Sockets.hpp"
#include "src\TCP\Client.hpp"
#include "src\TCP\Utils\Messages.hpp"
#include "src\TCP\Utils\Errors.hpp"

/* Construtor. Initialize counter. */
Communication::Communication()
{

}

ArPose Communication::getPoseFromStr(std::string str_pos) {
	ArPose tmp;
	//std::cout << str_pos;
	int i = 0;
	std::istringstream iss(str_pos);
	std::string token;
	while (std::getline(iss, token, ';'))
	{
		if (i == 0) tmp.setX(std::stod(token));
		else if (i == 1) tmp.setY(std::stod(token));
		else if (i == 2) tmp.setTh(std::stod(token));
		i++;
	}

	return tmp;
}

void* Communication::runThread(void*)
{
	str_message = "";

	if (!Network::Start())
	{
		std::cout << "Error starting sockets : " << Network::Errors::Get() << std::endl;
	}

	Network::TCP::Client client;

	unsigned short port = 300;

	if (!client.connect("10.1.22.23", port))
	{
		std::cout << "Impossible de se connecter au serveur [10.1.22.23:" << port << "] : " << Network::Errors::Get() << std::endl;
	}

	else
	{
		while (1)
		{
			while (auto msg = client.poll())
			{
				if (msg->is<Network::Messages::Connection>())
				{
					auto connection = msg->as<Network::Messages::Connection>();
					if (connection->result == Network::Messages::Connection::Result::Success)
					{
						std::cout << "Connecte !" << std::endl;
					}
					else
					{
						std::cout << "Connexion echoue : " << static_cast<int>(connection->result) << std::endl;
						break;
					}
				}
				else if (msg->is<Network::Messages::UserData>()) {
					auto userdata = msg->as<Network::Messages::UserData>();
					std::string reply(reinterpret_cast<const char*>(userdata->data.data()), userdata->data.size());
					std::istringstream iss(reply);
					std::string token;
					bool reception = false, position = false, posC = false, posD = false;
					while (std::getline(iss, token, ':'))
					{
						if (reception && token == "PosR") {
							position = true;
							bool_posR = true;
						}
						else if (reception && token == "PosC") {
							posC = true;
							bool_posC = true;
						}
						else if (reception && token == "PosD") {
							posD = true;
							bool_posD = true;
						}
						else if (reception && token == "Charge") {
							charge = true;
						}
						else if (reception && token == "Decharge") {
							decharge = true;
						}
						else if (reception && token == "Off") {
							stop = true;
						}
						else if (reception && token == "On") {
							stop = false;
						}
						else if (position) {
							Ar_posR = getPoseFromStr(token);
							/*
							robot.lock();
							robot.moveTo(Ar_posR);
							robot.unlock();
							*/
						}
						else if (posC) {
							Ar_posC = getPoseFromStr(token);
						}
						else if (posD) {
							Ar_posD = getPoseFromStr(token);
						}
						else if (token == "Amigo2" || token == "Global") reception = true;
					}
				}
				else if (msg->is<Network::Messages::Disconnection>())
				{
					auto disconnection = msg->as<Network::Messages::Disconnection>();
					std::cout << "Deconnecte : " << static_cast<int>(disconnection->reason) << std::endl;
					break;
				}
			}

			if (str_message != "") {
				myMutex.lock();

				if (!client.send(reinterpret_cast<const unsigned char*>(str_message.c_str()), static_cast<unsigned int>(str_message.length())))
				{
					std::cout << "Erreur envoi : " << Network::Errors::Get() << std::endl;
					break;
				}
				str_message = "";

				myMutex.unlock();
			}

			ArUtil::sleep(1000);
		}
	}
	Network::Release();

	ArLog::log(ArLog::Normal, "communication thread: requested stop running, ending thread.");
	return NULL;
}

ArPose Communication::getPosC() {
	return Ar_posC;
}

bool Communication::getBoolPosC() {
	if ((bool_posC) && (Communication::getPosC().getX() != -1)) {
		bool_posC = false;
		return true;
	}
	else return false;
}

ArPose Communication::getPosD() {
	return Ar_posD;
}

bool Communication::getBoolPosD() {
	if ((bool_posD) && (Communication::getPosD().getX() != -1)) {
		bool_posD = false;
		return true;
	}
	else return false;
}

ArPose Communication::getPosR() {
	return Ar_posR;
}

bool Communication::getBoolPosR() {
	if ((bool_posR) && (Communication::getPosR().getX() != -1)) {
		bool_posR = false;
		return true;
	}
	else return false;
}

bool Communication::getCharge() {
	if (charge) {
		charge = false;
		return true;
	}
	else return false;
}

bool Communication::getDecharge() {
	if (decharge) {
		cout << "decharge : " << 1 << endl;
		decharge = false;
		return true;
	}
	else return false;
}

bool Communication::getStop()
{
	return stop;
}

void Communication::setMessage(std::string message) {
	myMutex.lock();
	str_message = message;
	myMutex.unlock();
}

