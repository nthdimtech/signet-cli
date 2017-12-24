#ifndef UNLOCKTASK_H
#define UNLOCKTASK_H

#include "signettask.h"

class unlockTask : public signetTask
{
	void doLogin();
public:
	unlockTask();
	bool start();
	bool canStart();
	void cmdResponse(void *cb_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data);
};

#endif // LOGINTASK_H
