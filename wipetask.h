#ifndef WIPETASK_H
#define WIPETASK_H

#include "signettask.h"

class wipeTask : public signetTask
{
public:
	wipeTask();
	void cmdResponse(void *cmd_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data);
	bool canStart();
	bool start();
	static void help();
};

#endif // WIPETASK_H
