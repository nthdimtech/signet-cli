#ifndef SIGNETCLIAPPLICATION_H
#define SIGNETCLIAPPLICATION_H

#include "qtsinglecoreapplication.h"

#include <QString>
#include <QByteArray>

extern "C" {
#include "signetdev/host/signetdev.h"
}

class SignetCLIApplication : public QtSingleCoreApplication
{
	Q_OBJECT
	static SignetCLIApplication *g_singleton;

	static void generateScryptKey(const QString &password, u8 *key, const u8 *salt, unsigned int N, unsigned int r, unsigned int s);
public:
	static SignetCLIApplication *get()
	{
		return g_singleton;
	}
	static void generateKey(const QString &password, u8 *key, const u8 *hashfn, const u8 *salt);
	SignetCLIApplication(int &argc, char **argv);
};

#endif // SIGNETCLIAPPLICATION_H
