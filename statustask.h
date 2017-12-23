#ifndef STATUSTASK_H
#define STATUSTASK_H

#include "signettask.h"

class statusTask : public signetTask
{
public:
	statusTask();
	bool start();
	bool canStart();
	void cmdResponse(void *cb_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data);
};

#endif // STATUSTASK_H
