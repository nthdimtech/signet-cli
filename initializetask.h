#ifndef INITIALIZETASK_H
#define INITIALIZETASK_H

extern "C" {
#include "signetdev/host/signetdev.h"
}

#include "signettask.h"

class initializeTask : public signetTask
{
public:
	initializeTask();
	bool start();
	bool canStart();
	void cmdResponse(void *cb_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data);
	static void help();
};

#endif // INITIALIZETASK_H
