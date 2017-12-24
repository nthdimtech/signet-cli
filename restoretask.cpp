#include "restoretask.h"

#include "signetcliapplication.h"


extern "C" {
#include "signetdev/host/signetdev.h"
}

#include <iostream>
#include <string>

restoreTask::restoreTask(int argc, char *argv[])
{
	if (argc > 0) {
		m_file.setFileName(QString(argv[0]));
		bool rc = m_file.open(QIODevice::ReadOnly);
		if (!rc) {
			std::cout << "Failed to open \"" << argv[0] << "\"" << std::endl;
		} else {
			if (m_file.size() != (NUM_STORAGE_BLOCKS * BLK_SIZE)) {
				std::cout << "Backup file \"" << argv[0] << "\" is the wrong size" << std::endl;
				m_file.close();
			}
		}
		m_blockNum = 0;
	} else {
		std::cout << "No restore source file specified" << std::endl;
	}
}

bool restoreTask::canStart()
{
	return m_file.isOpen();
}

bool restoreTask::start()
{
	int token;
	::signetdev_get_device_state(NULL, &token);
	return true;
}

void restoreTask::cmdResponse(void *cb_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data)
{
	Q_UNUSED(cb_user_param);
	Q_UNUSED(cmd_token);
	Q_UNUSED(messages_remaining);
	Q_UNUSED(end_device_state);
	int token;
	switch(cmd) {
	case SIGNETDEV_CMD_STARTUP: {
		std::cout << "Press and HOLD the device button to begin restoring backup" << std::endl;
		::signetdev_begin_device_restore(NULL, &token);
		} break;
	case SIGNETDEV_CMD_GET_DEVICE_STATE: {
		switch (end_device_state) {
		case LOGGED_OUT:
		case UNINITIALIZED:
			std::cout << "Press and HOLD the device button to begin restoring backup" << std::endl;
			::signetdev_begin_device_restore(NULL, &token);
			break;
		case DISCONNECTED:
			::signetdev_startup(NULL, &token);
			break;
		case LOGGED_IN:
			std::cout << "Device is locked. The device must be unlocked to restore a backup" << std::endl;
			QCoreApplication::quit();
			break;
		default:
			std::cout << "The device must be unlocked to restore a backup" << std::endl;
			QCoreApplication::quit();
			break;
		}
		} break;
	case SIGNETDEV_CMD_BEGIN_DEVICE_RESTORE: {
		switch (resp_code) {
		case OKAY: {
			std::cout << "Restoring backup, this make take awhile..." << std::endl;
			QByteArray block = m_file.read(BLK_SIZE);
			if (block.size() != BLK_SIZE) {
				std::cout << "Failed to read from backup file" << std::endl;
				::signetdev_end_device_restore(NULL, &token);
			} else {
				::signetdev_write_block(NULL, &token, m_blockNum, block.data());
			}
			} break;
		default:
			std::cout << "Failed to start restoring backup up file" << std::endl;
			QCoreApplication::quit();
		}
		} break;
	case SIGNETDEV_CMD_WRITE_BLOCK: {
		switch (resp_code) {
		case OKAY: {
			m_blockNum++;
			if (m_blockNum == NUM_STORAGE_BLOCKS) {
				std::cout << "Backup restoration complete" << std::endl;
				::signetdev_end_device_restore(NULL, &token);
			} else {
				QByteArray block = m_file.read(BLK_SIZE);
				if (block.size() != BLK_SIZE) {
					std::cout << "Failed to read from backup file" << std::endl;
					::signetdev_end_device_restore(NULL, &token);
				} else {
					::signetdev_write_block(NULL, &token, m_blockNum, block.data());
				}
			}
			} break;
		default:
			std::cout << "Failed to write block while restoreing backup" << std::endl;
			::signetdev_end_device_restore(NULL, &token);
			break;
		}
		} break;
	case SIGNETDEV_CMD_END_DEVICE_RESTORE:
		m_file.close();
		QCoreApplication::quit();
		break;
	}
}

