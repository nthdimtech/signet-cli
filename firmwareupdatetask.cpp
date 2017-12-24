#include "firmwareupdatetask.h"

#include <QCoreApplication>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

extern "C" {
#include "signetdev/host/signetdev.h"
}

#include <iostream>

void firmwareUpdateTask::help()
{
	std::cout << "Update the device's firmware" << std::endl
		  << std::endl
		  << "Usage: signet-cli update-firmware <firmare file>" << std::endl << std::endl;
}

firmwareUpdateTask::firmwareUpdateTask(int argc, char *argv[])  : m_writePhase(false)
{
	if (argc <= 0) {
		m_canStart = false;
		fprintf(stderr, "No firmware file specified\n");
		return;
	}
	if (!m_fw.fromFile(argv[0])) {
		fprintf(stderr, "Firmware file invalid\n");
		m_canStart = false;
		return;
	}
	m_writingSectionIter = m_fw.fwSections.cbegin();
	m_writingAddr = m_writingSectionIter->lma;
	m_writingSize = 0;
	m_canStart = true;
}

bool firmwareUpdateTask::canStart()
{
	return m_canStart;
}

bool firmwareUpdateTask::start()
{
	int token;
	if (!m_canStart)
		return false;
	::signetdev_get_device_state(NULL, &token);
	return true;
}

void firmwareUpdateTask::sendFirmwareWriteCmd()
{
	int token;
	bool advance = false;
	unsigned int section_lma = m_writingSectionIter->lma;
	unsigned int section_size = m_writingSectionIter->size;
	unsigned int section_end = section_lma + section_size;
	unsigned int write_size = 1024;
	if ((m_writingAddr + write_size) >= section_end) {
		write_size = section_end - m_writingAddr;
		advance = true;
	}
	const void *data = m_writingSectionIter->contents.data() + (m_writingAddr - section_lma);

	::signetdev_write_flash(NULL, &token, m_writingAddr, data, write_size);
	if (advance) {
		m_writingSectionIter++;
		if (m_writingSectionIter != m_fw.fwSections.end()) {
			m_writingAddr = m_writingSectionIter->lma;
		}
	} else {
		m_writingAddr += write_size;
	}
	m_writingSize = write_size;
}

void firmwareUpdateTask::cmdResponse(void *cb_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data)
{
	Q_UNUSED(cb_user_param);
	Q_UNUSED(cmd_token);
	Q_UNUSED(messages_remaining);
	int token;
	switch(cmd) {
	case SIGNETDEV_CMD_GET_DEVICE_STATE: {
		switch (end_device_state) {
		case DISCONNECTED:
			::signetdev_startup(NULL, &token);
			break;
		case UNINITIALIZED:
		case LOGGED_IN:
			::signetdev_begin_update_firmware(NULL, &token);
			break;
		case LOGGED_OUT:
			std::cout << "You must unlock the device before updating firmware" << std::endl;
			QCoreApplication::quit();
			break;
		default:
			std::cout << "Device not in valid state for updating firmware" << std::endl;
			QCoreApplication::quit();
			break;
		}
	} break;
	case SIGNETDEV_CMD_STARTUP: {
		std::cout << "Press and HOLD device button to begin updating firmware" << std::endl;
		::signetdev_begin_update_firmware(NULL, &token);
		switch (end_device_state) {
		case LOGGED_OUT:
			std::cout << "You must unlock the device before updating firmware" << std::endl;
			QCoreApplication::quit();
			break;
		case UNINITIALIZED:
			::signetdev_begin_update_firmware(NULL, &token);
			break;
		default:
			std::cout << "Device not in valid state for updating firmware" << std::endl;
			QCoreApplication::quit();
			break;
		}

	} break;
	case SIGNETDEV_CMD_BEGIN_UPDATE_FIRMWARE: {
		std::cout << "Firmware update started, this make take awhile. "
			  << "Disconnecting your device before the update finishes will lead to device malfunction" << std::endl;

		QByteArray erase_pages_;
		QByteArray page_mask(512, 0);

		if (resp_code != OKAY) {
			printf("Begin firmware update failed. Code %d\n", resp_code);
			QCoreApplication::quit();
			return;
		}

		for (auto iter = m_fw.fwSections.begin(); iter != m_fw.fwSections.end(); iter++) {
			const fwSection &section = (*iter);
			unsigned int lma = section.lma;
			unsigned int lma_end = lma + section.size;
			int page_begin = (lma - 0x8000000)/2048;
			int page_end = (lma_end - 1 - 0x8000000)/2048;
			for (int i  = page_begin; i <= page_end; i++) {
				if (i < 0)
					continue;
				if (i >= 511)
					continue;
				page_mask[i] = 1;
			}
		}

		for (int i = 0; i < 512; i++) {
			if (page_mask[i]) {
				erase_pages_.push_back(i);
			}
		}
		::signetdev_erase_pages(NULL, &token, erase_pages_.size(), (u8 *)erase_pages_.data());
		} break;
	case SIGNETDEV_CMD_ERASE_PAGES:
		if (resp_code == OKAY) {
			::signetdev_get_progress(NULL, &token, 0, ERASING_PAGES);
		}
		break;
	case SIGNETDEV_CMD_WRITE_FLASH:
		if (resp_code == OKAY) {
			if (m_writingSectionIter == m_fw.fwSections.end()) {
				::signetdev_reset_device(NULL, &token);
			} else {
				sendFirmwareWriteCmd();
			}
		}
		break;
	case SIGNETDEV_CMD_GET_PROGRESS: {
		signetdev_get_progress_resp_data *resp = (signetdev_get_progress_resp_data *)resp_data;
		if (end_device_state == FIRMWARE_UPDATE) {
			m_writingSectionIter = m_fw.fwSections.begin();
			m_writingAddr = m_writingSectionIter->lma;
			sendFirmwareWriteCmd();
		} else {
			::signetdev_get_progress(NULL, &token, resp->total_progress, ERASING_PAGES);
		}
		} break;
	}
}

bool firmware::fromFile(const QString &path)
{
	QFile firmwareUpdateFile(path);
	bool result = firmwareUpdateFile.open(QFile::ReadWrite);
	if (!result) {
		firmwareUpdateFile.close();
		return false;
	}

	QByteArray datum = firmwareUpdateFile.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(datum);

	firmwareUpdateFile.close();

	bool validFw = !doc.isNull() && doc.isObject();

	QJsonObject docObj;
	QJsonObject sectionsObj;

	if (validFw) {
		docObj = doc.object();
		QJsonValue tempVal = docObj.value("sections");
		validFw = (tempVal != QJsonValue::Undefined) && tempVal.isObject();
		if (validFw) {
			sectionsObj = tempVal.toObject();
		}
	}

	if (validFw) {
		for (auto iter = sectionsObj.constBegin(); iter != sectionsObj.constEnd() && validFw; iter++) {
			fwSection section;
			section.name = iter.key();
			QJsonValue temp = iter.value();
			if (!temp.isObject()) {
				validFw = false;
				break;
			}

			QJsonObject sectionObj = temp.toObject();
			QJsonValue lmaVal = sectionObj.value("lma");
			QJsonValue sizeVal = sectionObj.value("size");
			QJsonValue contentsVal = sectionObj.value("contents");

			if (lmaVal == QJsonValue::Undefined ||
			    sizeVal == QJsonValue::Undefined ||
			    contentsVal == QJsonValue::Undefined ||
			    !lmaVal.isDouble() ||
			    !sizeVal.isDouble() ||
			    !contentsVal.isString()) {
				validFw = false;
				break;
			}
			section.lma = (unsigned int)(lmaVal.toDouble());
			section.size = (unsigned int)(sizeVal.toDouble());
			section.contents = QByteArray::fromBase64(contentsVal.toString().toLatin1());
			if (section.contents.size() != section.size) {
				validFw = false;
				break;
			}
			fwSections.append(section);
		}
	}

	if (!validFw)
		return 0;

	return validFw;
}
