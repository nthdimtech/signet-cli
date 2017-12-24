#include "initializetask.h"
#include "wipetask.h"

#include "signetcliapplication.h"

extern "C" {
#include "signetdev/host/signetdev.h"
}

#include <string>
#include <iostream>

void initializeTask::help()
{
	std::cout << "Initialize the device. Erases any existing contents."<< std::endl
		  << std::endl
		  << "Usage: signet-cli initialize" << std::endl  << std::endl;
}

initializeTask::initializeTask()
{

}

bool initializeTask::canStart()
{
	return true;
}

bool initializeTask::start()
{
	int token;
	::signetdev_startup(NULL, &token);
	return true;
}

void initializeTask::cmdResponse(void *cb_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data)
{
	Q_UNUSED(cb_user_param);
	Q_UNUSED(cmd_token);
	Q_UNUSED(messages_remaining);
	Q_UNUSED(end_device_state);
	int token;
	switch(cmd) {
	case SIGNETDEV_CMD_STARTUP: {
		signetdev_startup_resp_data *resp = (signetdev_startup_resp_data *) resp_data;
		std::string str;
		std::cout << "Enter password:";
		std::cout.flush();
		std::getline(std::cin, str);
		QString password(str.c_str());
		std::cout << "Repeat password:";
		std::cout.flush();
		std::getline(std::cin, str);
		QString passwordRepeat(str.c_str());

		if (password.compare(passwordRepeat)) {
			std::cout << "Passwords don't match" << std::endl;
			QCoreApplication::quit();
			return;
		}
		std::random_device rd;

		SignetCLIApplication *app = SignetCLIApplication::get();
		u8 key[LOGIN_KEY_SZ];
		u8 salt[SALT_SZ_V2];
		u8 rand_data[INIT_RAND_DATA_SZ];
		u8 hashfn[HASH_FN_SZ];

		for (int i = 0; i < (SALT_SZ_V2/4); i++) {
			((u32 *)salt)[i] = (u32)rd();
		}
		for (int i = 0; i < (INIT_RAND_DATA_SZ/4); i++) {
			((u32 *)rand_data)[i] = rd();
		}

		memset(hashfn, 0, HASH_FN_SZ);
		hashfn[0] = 1;
		hashfn[1] = 12;
		hashfn[2] = 32;
		hashfn[3] = 0;
		hashfn[4] = 1;

		std::cout << "Please wait while authentication key is being generated" << std::endl;
		app->generateKey(password, key, hashfn, salt);

		std::cout << "Press and hold the device button start initializing your device..." << std::endl;
		::signetdev_begin_initialize_device(NULL,
			&token,
			key, LOGIN_KEY_SZ,
			hashfn, HASH_FN_SZ,
			salt, SALT_SZ_V2,
			rand_data, INIT_RAND_DATA_SZ);
	} break;
	case SIGNETDEV_CMD_BEGIN_INITIALIZE_DEVICE: {
		if (resp_code != OKAY) {
			switch (resp_code) {
			default:
				std::cout << "Initialize device failed. Code " << resp_code << std::endl;
				break;
			}
			QCoreApplication::quit();
			return;
		} else {
			std::cout << "Initializing the device, this may take awhile...." << std::endl;
			::signetdev_get_progress(NULL, &token, 0, INITIALIZING);
		}
		} break;
	case SIGNETDEV_CMD_GET_PROGRESS: {
		signetdev_get_progress_resp_data *resp = (signetdev_get_progress_resp_data *)resp_data;
		switch (end_device_state) {
		case INITIALIZING:
			::signetdev_get_progress(NULL, &token, resp->total_progress, ERASING_PAGES);
			break;
		case LOGGED_OUT:
			std::cout << "Device initialization complete" << std::endl;
			QCoreApplication::quit();
			break;
		default:
			std::cout << "Device initialization completed in error" << std::endl;
			QCoreApplication::quit();
		}
		}break;
	}
}
