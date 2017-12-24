#ifndef FIRMWAREUPDATETASK_H
#define FIRMWAREUPDATETASK_H

#include "signettask.h"
#include <QString>
#include <QByteArray>
#include <QList>


struct fwSection {
	QString name;
	unsigned int lma;
	int size;
	QByteArray contents;
};

struct firmware {
	int majVer;
	int minVer;
	int stepVer;
	QList<fwSection> fwSections;
	bool fromFile(const QString &path);
};

class firmwareUpdateTask : public signetTask {
	firmware m_fw;
	QList<fwSection>::const_iterator m_writingSectionIter;
	int m_writingAddr;
	int m_writingSize;
	bool m_writePhase;
	bool m_canStart;
	void sendFirmwareWriteCmd();
public:
	firmwareUpdateTask(int argc, char *argv[]);

	void cmdResponse(void *cmd_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data);
	bool canStart();
	bool start();
};


#endif // FIRMWAREUPDATETASK_H
