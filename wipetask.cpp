#include "wipetask.h"

#include <QCoreApplication>


extern "C" {
#include "signetdev/host/signetdev.h"
}

#include <iostream>

void wipeTask::help()
{
	std::cout << "Wipe the device. Permanently erases all data." << std::endl
		  << std::endl
		  << "Usage: signet-cli wipe" << std::endl << std::endl;
}

wipeTask::wipeTask()
{
}

bool wipeTask::canStart()
{
	return true;
}

bool wipeTask::start()
{
	int token;
	::signetdev_startup(NULL, &token);
	return true;
}

void wipeTask::cmdResponse(void *cb_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data)
{
	Q_UNUSED(cb_user_param);
	Q_UNUSED(cmd_token);
	Q_UNUSED(messages_remaining);
	int token;
	switch(cmd) {
	case SIGNETDEV_CMD_STARTUP: {
		::signetdev_wipe(NULL, &token);
		fprintf(stdout, "Press and hold the device button to start the wipe operation\n");
	} break;
	case SIGNETDEV_CMD_WIPE: {
		if (resp_code != OKAY) {
			switch (resp_code) {
			case INVALID_STATE:
				fprintf(stderr, "Device already wiped. No action taken\n");
				break;
			default:
				fprintf(stderr, "Wipe failed. Code %d\n", resp_code);
				break;
			}
			QCoreApplication::quit();
			return;
		} else {
			fprintf(stdout, "Starting wipe operation, this will take some time\n");
			::signetdev_get_progress(NULL, &token, 0, WIPING);
		}
		} break;
	case SIGNETDEV_CMD_GET_PROGRESS: {
		signetdev_get_progress_resp_data *resp = (signetdev_get_progress_resp_data *)resp_data;
		if (end_device_state == WIPING) {
			::signetdev_get_progress(NULL, &token, resp->total_progress, ERASING_PAGES);
		} else {
			fprintf(stdout, "Wipe completed\n");
			QCoreApplication::quit();
		}
		} break;
	}
}
