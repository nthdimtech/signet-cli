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

#define VERSION_STRING "0.0.2"
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
		  << "\t" APPLICATION_STRING " help <command>" << std::endl;
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

	if (argc < 2) {
		noCommandSpecified();
		return -1;
	}

	QString command = QString(argv[1]);

	if (!g_commandList.contains(command)) {
		unknownCommand(command);
		return -1;
	}

	signetTask *task = NULL;

	argc -= 2;
	argv += 2;

	if (!command.compare("update-firmware")) {
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
	//TODO:
	// backup device
	// restore device
	// interactive mode
	// login
	// logout
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

	return app.exec();
}
