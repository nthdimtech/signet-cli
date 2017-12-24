#ifndef BACKUPTASK_H
#define BACKUPTASK_H

#include "signettask.h"

#include <QFile>

class backupTask : public signetTask
{
	QFile m_file;
	int m_blockNum;
public:
	backupTask(int argc, char *argv[]);
	bool start();
	bool canStart();
	void cmdResponse(void *cb_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data);
	static void help();
};

#endif // BACKUPTASK_H
