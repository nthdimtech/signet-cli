QT += core network
QT -= gui

CONFIG += c++11

TARGET = signet-cli
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

CONFIG(release, debug|release):LIBS += -L$$PWD/signet-firmware/build-signetdev-$$QT_ARCH-release
CONFIG(debug, debug|release):LIBS += -L$$PWD/signet-firmware/build-signetdev-$$QT_ARCH-debug
LIBS += -lsignetdev
INCLUDEPATH += $$PWD/signet-firmware
INCLUDEPATH += $$PWD/qtsingleapplication/src

SOURCES += main.cpp \
    firmwareupdatetask.cpp \
    qtsingleapplication/src/qtlocalpeer.cpp \
    qtsingleapplication/src/qtlockedfile_unix.cpp \
    qtsingleapplication/src/qtlockedfile_win.cpp \
    qtsingleapplication/src/qtsinglecoreapplication.cpp

HEADERS += \
    firmwareupdatetask.h \
    signettask.h \
    qtsingleapplication/src/qtsinglecoreapplication.h \
    qtsingleapplication/src/qtlockedfile.h \
    qtsingleapplication/src/qtlocalpeer.h

