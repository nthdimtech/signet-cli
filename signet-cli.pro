QT += core network
QT -= gui

CONFIG += c++11

TARGET = signet-cli
CONFIG += console
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -std=c++11 -msse4.1

TEMPLATE = app

CONFIG(release, debug|release):LIBS += -L$$PWD/signet-firmware/build-signetdev-$$QT_ARCH-release
CONFIG(debug, debug|release):LIBS += -L$$PWD/signet-firmware/build-signetdev-$$QT_ARCH-debug
LIBS += -lsignetdev
INCLUDEPATH += $$PWD/signet-firmware
INCLUDEPATH += $$PWD/qtsingleapplication/src
INCLUDEPATH += $$PWD/scrypt

SOURCES += main.cpp \
    firmwareupdatetask.cpp \
    qtsingleapplication/src/qtlocalpeer.cpp \
    qtsingleapplication/src/qtsinglecoreapplication.cpp \
    wipetask.cpp \
    changepasswordtask.cpp \
    signetcliapplication.cpp \
    scrypt/crypto_scrypt_smix_sse2.c \
    scrypt/crypto_scrypt_smix.c \
    scrypt/crypto_scrypt.c \
    scrypt/insecure_memzero.c \
    scrypt/sha256.c \
    scrypt/warnp.c \
    initializetask.cpp \
    statustask.cpp \
    unlocktask.cpp \
    locktask.cpp

win32 {
SOURCES += qtsingleapplication/src/qtlockedfile_win.cpp
}

unix {
SOURCES += qtsingleapplication/src/qtlockedfile_unix.cpp
}

HEADERS += \
    firmwareupdatetask.h \
    signettask.h \
    qtsingleapplication/src/qtsinglecoreapplication.h \
    qtsingleapplication/src/qtlockedfile.h \
    qtsingleapplication/src/qtlocalpeer.h \
    wipetask.h \
    changepasswordtask.h \
    signetcliapplication.h \
    initializetask.h \
    statustask.h \
    unlocktask.h \
    locktask.h

