#ifndef SIGNETTASK_H
#define SIGNETTASK_H

class signetTask {
public:
	virtual void cmdResponse(void *cb_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data) = 0;
	virtual bool canStart() = 0;
	virtual bool start() = 0;
	virtual ~signetTask() {
	}
};

#endif // SIGNETTASK_H
