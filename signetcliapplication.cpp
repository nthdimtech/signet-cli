#include "signetcliapplication.h"

extern "C" {
#include "signetdev/host/signetdev.h"
#include "crypto_scrypt.h"
};

#include "qtsinglecoreapplication.h"

SignetCLIApplication *SignetCLIApplication::g_singleton = NULL;

SignetCLIApplication::SignetCLIApplication(int &argc, char **argv) :
	QtSingleCoreApplication("qtsingle-app-signetdev-" + QString(USB_VENDOR_ID) + "-" + QString(USB_SIGNET_DESKTOP_PRODUCT_ID) ,argc, argv)
{
	g_singleton = this;
}

void SignetCLIApplication::generateScryptKey(const QString &password, QByteArray &key, const QByteArray &salt, unsigned int N, unsigned int r, unsigned int s)
{
	QByteArray password_utf8 = password.toUtf8();
	crypto_scrypt((u8 *)password_utf8.data(), password_utf8.size(),
		      (u8 *)salt.data(), salt.size(),
		      N, r, s,
		      (u8 *)key.data(), key.size());

}

void SignetCLIApplication::generateKey(const QString &password, QByteArray &key, const QByteArray &hashfn, const QByteArray &salt, int keyLength)
{
	QByteArray s = password.toUtf8();
	key.resize(keyLength);
	memset(key.data(), 0, key.length());

	int fn = hashfn.at(0);

	switch(fn) {
	case 1: {
		if (hashfn.size() >= 5) {
			unsigned int N = ((unsigned int )1) << hashfn.at(1);
			unsigned int r = ((unsigned int)hashfn.at(2)) + (((unsigned int)hashfn.at(3))<<8);
			unsigned int p = (unsigned int)hashfn.at(4);
			generateScryptKey(password, key, salt, N, r, p);
		}
	}
	break;
	default:
		break;
	}
}
