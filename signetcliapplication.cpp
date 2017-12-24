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

void SignetCLIApplication::generateScryptKey(const QString &password, u8 *key, const u8 *salt, unsigned int N, unsigned int r, unsigned int s)
{
	QByteArray password_utf8 = password.toUtf8();
	crypto_scrypt((u8 *)password_utf8.data(), password_utf8.size(),
		      salt, SALT_SZ_V2,
		      N, r, s,
		      key, LOGIN_KEY_SZ);

}

void SignetCLIApplication::generateKey(const QString &password, u8 *key, const u8 *hashfn, const u8 *salt)
{
	QByteArray s = password.toUtf8();
	memset(key, 0, LOGIN_KEY_SZ);

	int fn = hashfn[0];

	switch(fn) {
	case 1: {
		unsigned int N = ((unsigned int )1) << hashfn[1];
		unsigned int r = ((unsigned int)hashfn[2]) + (((unsigned int)hashfn[3])<<8);
		unsigned int p = (unsigned int)hashfn[4];
		generateScryptKey(password, key, salt, N, r, p);
	}
	break;
	default:
		break;
	}
}
