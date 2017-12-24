#include <QCoreApplication>
#include <QFile>
#include <QTimer>
#include <QString>
#include <QList>
#include <QStringList>

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

#define VERSION_STRING "0.0.1"
#define APPLICATION_STRING "signet-cli"

QStringList g_commandList;

void deviceClosedS(void *user)
{
	Q_UNUSED(user);
	fprintf(stdout, "Device closed\n");
	QCoreApplication::quit();
}

void connectionErrorS(void *this_)
{
	Q_UNUSED(this_);
	fprintf(stdout, "Connection closed\n");
	QCoreApplication::quit();
}

void signetCmdResponse(void *cb_param, void *cmd_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data)
{
	signetTask *task = (signetTask *)cb_param;
	task->cmdResponse(cmd_user_param, cmd_token, cmd, end_device_state, messages_remaining, resp_code, resp_data);
}

void unknownCommand(QString command)
{
	fprintf(stderr,	"Unknown command %s", command.toLatin1().data());
}

void noCommandSpecified()
{
	fprintf(stderr, "No command specified. Possible commands are: \n");
	QList<QString>::iterator iter;
	for (iter = g_commandList.begin(); iter != g_commandList.end(); iter++) {
		fprintf(stderr, "\t%s\n", (*iter).toLatin1().data());
	}
	fprintf(stderr,
		"\nFor help regarding a specific command run:\n"
		"\t" APPLICATION_STRING " help <command>\n");
}

int main(int argc, char *argv[])
{
	SignetCLIApplication app(argc, argv);
	fprintf(stdout, "\n" APPLICATION_STRING " version " VERSION_STRING "\n\n");

	if (app.isRunning()) {
		fprintf(stderr,
			"A Signet client instance is already running. Close it and try again\n");
		return -1;
	}

	g_commandList.append("update-firmware");
	g_commandList.append("wipe");
	g_commandList.append("change-password");
	g_commandList.append("initialize");

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
	//TODO:
	// initialize
	// backup device
	// get device status
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
		fprintf(stdout, "No signet device detected\n");
		return -1;
	}

	::signetdev_set_command_resp_cb(signetCmdResponse, task);
	::signetdev_set_device_closed_cb(deviceClosedS, NULL);
	::signetdev_set_error_handler(connectionErrorS, NULL);

	task->start();

	return app.exec();
}
