#include "changepasswordtask.h"
#include "wipetask.h"

#include "signetcliapplication.h"


extern "C" {
#include "signetdev/host/signetdev.h"
}

#include <string>
#include <iostream>

changePasswordTask::changePasswordTask()
{
}

bool changePasswordTask::canStart()
{
	return true;
}

bool changePasswordTask::start()
{
	int token;
	::signetdev_startup(NULL, &token);
	return true;
}

void changePasswordTask::cmdResponse(void *cb_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data)
{
	Q_UNUSED(cb_user_param);
	Q_UNUSED(cmd_token);
	Q_UNUSED(messages_remaining);
	Q_UNUSED(end_device_state);
	int token;
	switch(cmd) {
	case SIGNETDEV_CMD_STARTUP: {
		signetdev_startup_resp_data *resp = (signetdev_startup_resp_data *) resp_data;
		std::cout << "Enter old password:";
		std::cout.flush();
		std::string str;
		std::getline(std::cin, str);
		QString oldPassword(str.c_str());
		std::cout << "Enter new password:";
		std::cout.flush();
		std::getline(std::cin, str);
		QString newPassword(str.c_str());
		std::cout << "Repeat new password:";
		std::cout.flush();
		std::getline(std::cin, str);
		QString newPasswordRepeat(str.c_str());

		if (newPassword.compare(newPasswordRepeat)) {
			std::cout << "New passwords don't match" << std::endl;
			QCoreApplication::quit();
			return;
		}

		SignetCLIApplication *app = SignetCLIApplication::get();
		QByteArray oldKey;
		QByteArray newKey;
		QByteArray salt((const char *)resp->salt, SALT_SZ_V2);
		QByteArray hashfn((const char *)resp->hashfn, HASH_FN_SZ);
		std::cout << "Please wait while authentication keys are being generated..." << std::endl;
		app->generateKey(oldPassword, oldKey, hashfn, salt, LOGIN_KEY_SZ);
		app->generateKey(newPassword, newKey, hashfn, salt, LOGIN_KEY_SZ);
		::signetdev_change_master_password(NULL, &token,
						   (u8 *)oldKey.data(), LOGIN_KEY_SZ,
						   (u8 *)newKey.data(), LOGIN_KEY_SZ,
						   (u8 *)hashfn.data(), HASH_FN_SZ,
						   (u8 *)salt.data(), SALT_SZ_V2);
		std::cout << "Press and hold the device button to change your master password..." << std::endl;
	} break;
	case SIGNETDEV_CMD_CHANGE_MASTER_PASSWORD: {
		if (resp_code != OKAY) {
			switch (resp_code) {
			case BAD_PASSWORD:
				std::cout << "Change password failed. Old password incorrect" << std::endl;
				break;
			default:
				std::cout << "Change password failed. Code " << resp_code << std::endl;
				break;
			}
			QCoreApplication::quit();
			return;
		} else {
			std::cout << "Password changed successfully" << std::endl;
			QCoreApplication::quit();
		}
		} break;
	}
}

