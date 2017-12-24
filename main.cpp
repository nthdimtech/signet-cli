#include <QCoreApplication>
#include <QFile>
#include <QTimer>
#include <QString>
#include <QList>
#include <QStringList>
#include <iostream>

#include "signetcliapplication.h"

#include <stdio.h>

extern "C" {
#include "signetdev/host/signetdev.h"
}

#include "signettask.h"
#include "firmwareupdatetask.h"
#include "changepasswordtask.h"
#include "initializetask.h"
#include "wipetask.h"
#include "statustask.h"
#include "unlocktask.h"
#include "locktask.h"
#include "backuptask.h"
#include "restoretask.h"

#define VERSION_STRING "0.0.4"
#define APPLICATION_STRING "signet-cli"

QStringList g_commandList;

void deviceClosedS(void *user)
{
	Q_UNUSED(user);
	std::cout << "Device closed" << std::endl;
	QCoreApplication::quit();
}

void connectionErrorS(void *this_)
{
	Q_UNUSED(this_);
	std::cout << "Connection closed" << std::endl;
	QCoreApplication::quit();
}

void signetCmdResponse(void *cb_param, void *cmd_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data)
{
	signetTask *task = (signetTask *)cb_param;
	task->cmdResponse(cmd_user_param, cmd_token, cmd, end_device_state, messages_remaining, resp_code, resp_data);
}

void unknownCommand(QString command)
{
	std::cout << "Unknown command: " << command.toLatin1().data() << std::endl;
}

void noCommandSpecified()
{
	std::cout << "No command specified. Possible commands are: " << std::endl;
	QList<QString>::iterator iter;
	for (iter = g_commandList.begin(); iter != g_commandList.end(); iter++) {
		std::cout << "\t" << (*iter).toLatin1().data() << std::endl;
	}
	std::cout << std::endl
		  << "For help regarding a specific command run:" << std::endl
		  << "\t" APPLICATION_STRING " help <command>" << std::endl << std::endl;
}

void help(int argc, char *argv[])
{
	if (argc <= 0) {
		std::cout << "Possible commands are: " << std::endl;
		QList<QString>::iterator iter;
		for (iter = g_commandList.begin(); iter != g_commandList.end(); iter++) {
			std::cout << "\t" << (*iter).toLatin1().data() << std::endl;
		}
		std::cout << std::endl
			  << "For help regarding a specific command run:" << std::endl
			  << "\t" APPLICATION_STRING " help <command>" << std::endl << std::endl;
	} else {
		QString command(argv[0]);
		if (!command.compare("update-firmware")) {
			firmwareUpdateTask::help();
		}
		if (!command.compare("wipe")) {
			wipeTask::help();
		}
		if (!command.compare("change-password")) {
			changePasswordTask::help();
		}
		if (!command.compare("initialize")) {
			initializeTask::help();
		}
		if (!command.compare("status")) {
			statusTask::help();
		}
		if (!command.compare("unlock")) {
			unlockTask::help();
		}
		if (!command.compare("lock")) {
			lockTask::help();
		}
		if (!command.compare("backup")) {
			backupTask::help();
		}
		if (!command.compare("restore")) {
			restoreTask::help();
		}

		if (!g_commandList.contains(command)) {
			std::cout << "Unknown command: " << argv[0] << std::endl << std::endl;
		}
	}
}

int main(int argc, char *argv[])
{
	SignetCLIApplication app(argc, argv);
	std::cout << std::endl << (APPLICATION_STRING " version " VERSION_STRING) << std::endl << std::endl;

	if (app.isRunning()) {
		std::cout <<  "A Signet client instance is already running. Close it and try again" << std::endl;
		return -1;
	}

	g_commandList.append("update-firmware");
	g_commandList.append("wipe");
	g_commandList.append("change-password");
	g_commandList.append("initialize");
	g_commandList.append("status");
	g_commandList.append("unlock");
	g_commandList.append("lock");
	g_commandList.append("backup");
	g_commandList.append("restore");
	if (argc < 2) {
		noCommandSpecified();
		return -1;
	}

	QString command = QString(argv[1]);
	argc -= 2;
	argv += 2;

	if (!command.compare("help")) {
		help(argc, argv);
		return 0;
	}

	if (!g_commandList.contains(command)) {
		unknownCommand(command);
		return -1;
	}

	signetTask *task = NULL;

	if (!command.compare("update-firmware")) {
		//TODO: we need the firmware update task to handle device
		//connect and disconnect messages so it doesn't look like
		//an error when the device auto-resets after the firmware
		//upgrade
		task = new firmwareUpdateTask(argc, argv);
	}
	if (!command.compare("wipe")) {
		task = new wipeTask();
	}
	if (!command.compare("change-password")) {
		task = new changePasswordTask();
	}
	if (!command.compare("initialize")) {
		task = new initializeTask();
	}
	if (!command.compare("status")) {
		task = new statusTask();
	}
	if (!command.compare("unlock")) {
		task = new unlockTask();
	}
	if (!command.compare("lock")) {
		task = new lockTask();
	}
	if (!command.compare("backup")) {
		task = new backupTask(argc, argv);
	}
	if (!command.compare("restore")) {
		task = new restoreTask(argc, argv);
	}

	//TODO:
	// backup device
	// restore device
	// interactive mode
	// list accounts
	// view account
	// set account
	// new account
	// type username
	// type password
	// type email
	// type username + password


	if (!task->canStart()) {
		return -1;
	}

	::signetdev_initialize_api();

	int rc = ::signetdev_open_connection();
	if (rc != OKAY) {
		::signetdev_deinitialize_api();
		std::cout << "No signet device detected" << std::endl;
		return -1;
	}

	::signetdev_set_command_resp_cb(signetCmdResponse, task);
	::signetdev_set_device_closed_cb(deviceClosedS, NULL);
	::signetdev_set_error_handler(connectionErrorS, NULL);

	task->start();

	rc = app.exec();
	delete task;
	::signetdev_deinitialize_api();
	return rc;
}
