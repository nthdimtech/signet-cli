#include "locktask.h"

#include "signetcliapplication.h"


extern "C" {
#include "signetdev/host/signetdev.h"
}

#include <string>
#include <iostream>


lockTask::lockTask()
{

}

bool lockTask::canStart()
{
	return true;
}

bool lockTask::start()
{
	int token;
	::signetdev_get_device_state(NULL, &token);
	return true;
}

void lockTask::cmdResponse(void *cb_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data)
{
	Q_UNUSED(cb_user_param);
	Q_UNUSED(cmd_token);
	Q_UNUSED(messages_remaining);
	Q_UNUSED(end_device_state);
	int token;
	switch(cmd) {
	case SIGNETDEV_CMD_GET_DEVICE_STATE: {
		switch (end_device_state) {
		case LOGGED_IN:
			::signetdev_logout(NULL, &token);
			break;
		default:
			std::cout << "Not locked" << std::endl;
			QCoreApplication::quit();
			break;
		}
		} break;
	case SIGNETDEV_CMD_LOGOUT:
		if (resp_code == OKAY) {
			std::cout << "Device locked" << std::endl;
		} else {
			std::cout << "Failed to lock device: error code " << resp_code << std::endl;
		}
		QCoreApplication::quit();
		break;
	}
}
