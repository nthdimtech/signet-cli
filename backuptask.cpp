#include "backuptask.h"

#include "signetcliapplication.h"


extern "C" {
#include "signetdev/host/signetdev.h"
}

#include <iostream>
#include <string>

backupTask::backupTask(int argc, char *argv[])
{
	if (argc > 0) {
		m_file.setFileName(QString(argv[0]));
		bool rc = m_file.open(QIODevice::WriteOnly);
		if (!rc) {
			std::cout << "Failed to open \"" << argv[0] << "\"" << std::endl;
		}
		m_blockNum = 0;
	} else {
		std::cout << "No backup destination file specified" << std::endl;
	}
}

bool backupTask::canStart()
{
	return m_file.isOpen();
}

bool backupTask::start()
{
	int token;
	::signetdev_get_device_state(NULL, &token);
	return true;
}

void backupTask::cmdResponse(void *cb_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data)
{
	Q_UNUSED(cb_user_param);
	Q_UNUSED(cmd_token);
	Q_UNUSED(messages_remaining);
	Q_UNUSED(end_device_state);
	int token;
	switch(cmd) {
	case SIGNETDEV_CMD_GET_DEVICE_STATE: {
		switch (end_device_state) {
		case LOGGED_IN:
			std::cout << "Press and HOLD the device button to begin backup" << std::endl;
			::signetdev_begin_device_backup(NULL, &token);
			break;
		default:
			std::cout << "The device must be unlocked to make a backup" << std::endl;
			QCoreApplication::quit();
			break;
		}
		} break;
	case SIGNETDEV_CMD_BEGIN_DEVICE_BACKUP: {
		switch (resp_code) {
		case OKAY:
			std::cout << "Performing backup, this make take awhile..." << std::endl;
			::signetdev_read_block(NULL, &token, m_blockNum);
			break;
		default:
			std::cout << "Failed to start backing up" << std::endl;
			QCoreApplication::quit();
		}
		} break;
	case SIGNETDEV_CMD_READ_BLOCK: {
		switch (resp_code) {
		case OKAY: {
			int d = m_file.write(QByteArray((const char *)resp_data, BLK_SIZE));
			m_blockNum++;
			if (m_blockNum == NUM_STORAGE_BLOCKS) {
				std::cout << "Backup complete" << std::endl;
				::signetdev_end_device_backup(NULL, &token);
			} else {
				::signetdev_read_block(NULL, &token, m_blockNum);
			}
			} break;
		default:
			std::cout << "Failed to read block while backing up" << std::endl;
			::signetdev_end_device_backup(NULL, &token);
			break;
		}
		} break;
	case SIGNETDEV_CMD_END_DEVICE_BACKUP:
		m_file.close();
		QCoreApplication::quit();
		break;
	}
}

