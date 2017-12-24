#ifndef RESTORETASK_H
#define RESTORETASK_H

#include "signettask.h"

#include <QFile>

class restoreTask : public signetTask
{
	QFile m_file;
	int m_blockNum;
public:
	restoreTask(int argc, char *argv[]);
	bool start();
	bool canStart();
	void cmdResponse(void *cb_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data);
};

#endif // RESTORETASK_H
