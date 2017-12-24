#include "unlocktask.h"

extern "C" {
#include "signetdev/host/signetdev.h"
}

#include "signetcliapplication.h"

#include <iostream>

unlockTask::unlockTask()
{
}

bool unlockTask::canStart()
{
	return true;
}

bool unlockTask::start()
{
	int token;
	::signetdev_get_device_state(NULL, &token);
	return true;
}

void unlockTask::doLogin()
{
	std::string str;
	std::cout << "Enter password:";
	std::cout.flush();
	std::cin >> str;
}

void unlockTask::cmdResponse(void *cb_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data)
{
	Q_UNUSED(cb_user_param);
	Q_UNUSED(cmd_token);
	Q_UNUSED(messages_remaining);
	int token;
	switch(cmd) {
	case SIGNETDEV_CMD_GET_DEVICE_STATE: {
		switch (end_device_state) {
		case LOGGED_IN:
			std::cout << "Device already unlocked" << std::endl;
			QCoreApplication::quit();
			break;
		default:
			::signetdev_startup(NULL, &token);
			break;
		}
		} break;
	case SIGNETDEV_CMD_STARTUP: {
		SignetCLIApplication *app = SignetCLIApplication::get();
		signetdev_startup_resp_data *resp = (signetdev_startup_resp_data *) resp_data;
		std::cout << "Enter password:";
		std::cout.flush();
		std::string str;
		std::getline(std::cin, str);
		u8 key[LOGIN_KEY_SZ];
		std::cout << "Please wait while the authenticatio key is being generated..." << std::endl;
		app->generateKey(QString(str.c_str()), key, resp->hashfn, resp->salt);
		std::cout << "Press device button to login..." << std::endl;
		::signetdev_login(NULL, &token, key, LOGIN_KEY_SZ, 0);
		} break;
	case SIGNETDEV_CMD_LOGIN:
		switch (resp_code) {
		case BAD_PASSWORD:
			std::cout << "Password invalid" << std::endl;
			break;
		case OKAY:
			std::cout << "Device unlocked" << std::endl;
			break;
		default:
			std::cout << "Unexpected error " << resp_code << std::endl;
			break;
		}
		QCoreApplication::quit();
		break;
	}
}
