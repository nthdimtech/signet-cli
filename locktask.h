#ifndef LOCKTASK_H
#define LOCKTASK_H

#include "signettask.h"

class lockTask : public signetTask
{
public:
	lockTask();
	bool start();
	bool canStart();
	void cmdResponse(void *cb_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data);
	static void help();
};

#endif // LOCKTASK_H
