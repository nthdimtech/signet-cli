#ifndef CHANGEPASSWORDTASK_H
#define CHANGEPASSWORDTASK_H

#include "signettask.h"

class changePasswordTask : public signetTask
{
public:
	changePasswordTask();
	void cmdResponse(void *cmd_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data);
	bool canStart();
	bool start();
	static void help();
};

#endif // CHANGEPASSWORDTASK_H
