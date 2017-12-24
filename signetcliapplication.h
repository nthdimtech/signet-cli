#ifndef SIGNETCLIAPPLICATION_H
#define SIGNETCLIAPPLICATION_H

#include "qtsinglecoreapplication.h"

#include <QString>
#include <QByteArray>

class SignetCLIApplication : public QtSingleCoreApplication
{
	Q_OBJECT
	static SignetCLIApplication *g_singleton;

	static void generateScryptKey(const QString &password, QByteArray &key, const QByteArray &salt, unsigned int N, unsigned int r, unsigned int s);
public:
	static SignetCLIApplication *get()
	{
		return g_singleton;
	}
	static void generateKey(const QString &password, QByteArray &key, const QByteArray &hashfn, const QByteArray &salt, int keyLength);
	SignetCLIApplication(int &argc, char **argv);
};

#endif // SIGNETCLIAPPLICATION_H
