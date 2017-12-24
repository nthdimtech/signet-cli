#include <QCoreApplication>
#include <QFile>
#include <QTimer>
#include <QString>
#include <QList>
#include <QStringList>

#include "qtsinglecoreapplication.h"

#include <stdio.h>

extern "C" {
#include "signetdev/host/signetdev.h"
}

#include "signettask.h"
#include "firmwareupdatetask.h"

#define VERSION_STRING "1.0.0"
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
	fprintf(stdout, "Programming complete\n");
	QCoreApplication::quit();
}

void signetCmdResponse(void *cb_param, void *cmd_user_param, int cmd_token, int cmd, int end_device_state, int messages_remaining, int resp_code, void *resp_data)
{
	signetTask *task = (signetTask *)cb_param;
	task->cmdResponse(cmd_user_param, cmd_token, cmd, end_device_state, messages_remaining, resp_code, resp_data);
}

void unknownCommand(QString command)
{
	fprintf(stderr,
		APPLICATION_STRING " version " VERSION_STRING "\n"
		"\n"
		"Unknown command %s",
		command.toLatin1().data());
}

void noCommandSpecified()
{
	fprintf(stderr,
		"\n" APPLICATION_STRING " version " VERSION_STRING "\n"
		"\n"
		"No command specified. Possible commands are: \n");
	QList<QString>::iterator iter;
	fprintf(stderr, "\thelp\n");
	for (iter = g_commandList.begin(); iter != g_commandList.end(); iter++) {
		fprintf(stderr, "\t%s\n", (*iter).toLatin1().data());
	}
}

int main(int argc, char *argv[])
{
	QtSingleCoreApplication *app = new QtSingleCoreApplication(
				"qtsingle-app-signetdev-" + QString(USB_VENDOR_ID) + "-" + QString(USB_SIGNET_DESKTOP_PRODUCT_ID),
				argc, argv);
	if (app->isRunning()) {
		fprintf(stderr,
			"\n" APPLICATION_STRING " version " VERSION_STRING "\n"
			"\n"
			"A Signet client instance is already running. Close it and try again\n");
		return -1;
	}

	g_commandList.append("update-firmware");

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
	//TODO:
	// initialize
	// get device status
	// change password
	// wipe
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
		fprintf(stdout, "No signet device detected\n");
		return -1;
	}

	::signetdev_set_command_resp_cb(signetCmdResponse, task);
	::signetdev_set_device_closed_cb(deviceClosedS, NULL);
	::signetdev_set_error_handler(connectionErrorS, NULL);

	task->start();

	return app->exec();
}
