QT += core network
QT -= gui

CONFIG += c++11

TARGET = signet-cli
CONFIG += console
CONFIG -= app_bundle

QMAKE_CXXFLAGS += -std=c++11 -msse4.1
QMAKE_CFLAGS += -std=c99 -msse4.1

TEMPLATE = app

INCLUDEPATH += $$PWD/signet-base
INCLUDEPATH += $$PWD/qtsingleapplication/src
INCLUDEPATH += $$PWD/scrypt

SOURCES += signet-base/signetdev/host/signetdev.c

unix {
HEADERS += signet-base/signetdev/host/signetdev_unix.h
SOURCES += signet-base/signetdev/host/signetdev_unix.c
}

win32 {
SOURCES += signet-base/signetdev/host/rawhid/hid_WINDOWS.c \
        signet-base/signetdev/host/signetdev_win32.c
}

macx {
SOURCES += signet-base/signetdev/host/signetdev_osx.c
HEADERS += signet-base/signetdev/host/signetdev_osx.h
}

unix:!macx {
SOURCES += signet-base/signetdev/host/signetdev_linux.c
}

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
    locktask.cpp \
    backuptask.cpp \
    restoretask.cpp

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
    locktask.h \
    backuptask.h \
    restoretask.h

