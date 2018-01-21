#include "statustask.h"

#include "signetcliapplication.h"


extern "C" {
#include "signetdev/host/signetdev.h"
}

#include <iostream>

void statusTask::help()
{
	std::cout << "Output's the device's current status" << std::endl
		  << std::endl
		  << "Usage: signet-cli status" << std::endl << std::endl;
}

statusTask::statusTask()
{
}

bool statusTask::canStart()
{
	return true;
}

bool statusTask::start()
{
	int token;
	::signetdev_get_device_state(NULL, &token);
	return true;
}

void statusTask::cmdResponse(void *cb_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data)
{
	Q_UNUSED(resp_code);
	Q_UNUSED(resp_data);
	Q_UNUSED(cb_user_param);
	Q_UNUSED(cmd_token);
	Q_UNUSED(messages_remaining);
	int token;
	switch(cmd) {
	case SIGNETDEV_CMD_GET_DEVICE_STATE: {
		if (end_device_state == DISCONNECTED) {
			::signetdev_startup(NULL, &token);
		} else {
			std::string str;
			SignetCLIApplication::get()->deviceStateToString(end_device_state, str);
			std::cout << "Device status: " << str << std::endl << std::endl;
			QCoreApplication::quit();
		}
		} break;
	case SIGNETDEV_CMD_STARTUP: {
		std::string str;
		SignetCLIApplication::get()->deviceStateToString(end_device_state, str);
		std::cout << "Device status: " << str << std::endl << std::endl;
		QCoreApplication::quit();
		} break;
	}
}
