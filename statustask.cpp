#include "statustask.h"

#include "signetcliapplication.h"


extern "C" {
#include "signetdev/host/signetdev.h"
}

#include <iostream>

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
	Q_UNUSED(cb_user_param);
	Q_UNUSED(cmd_token);
	Q_UNUSED(messages_remaining);
	int token;
	switch(cmd) {
	case SIGNETDEV_CMD_GET_DEVICE_STATE: {
		std::string str;
		SignetCLIApplication::get()->deviceStateToString(end_device_state, str);
		std::cout << "Device status: " << str << std::endl;
		QCoreApplication::quit();
		} break;
	}
}
