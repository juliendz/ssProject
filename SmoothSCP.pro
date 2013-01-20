#-------------------------------------------------
#
# Project created by QtCreator 2013-01-13T14:54:54
#
#-------------------------------------------------

QT       += core gui sql

TARGET = SmoothSCP
TEMPLATE = app


SOURCES += \
    src/main.cpp \
    src/net/sshsession.cpp \
    src/net/ls_channel.cpp \
    src/net/get_channel.cpp \
    src/sessions/SessionManager.cpp \
    src/sessions/Session.cpp \
    src/threads/ssWorker.cpp \
    src/ui/SessionsTableViewDelegate.cpp \
    src/ui/SessCreateForm.cpp \
    src/ui/MainWindow.cpp \
    src/ui/FMgr.cpp \
    src/logger.cpp \
    src/net/put_channel.cpp

HEADERS  += \
    src/common.h \
    src/interfaces/ISSHChannel.h \
    src/net/sshsession.h \
    src/net/ls_channel.h \
    src/net/get_channel.h \
    src/sessions/SessionManager.h \
    src/sessions/Session.h \
    src/threads/ssWorker.h \
    src/ui/SessionsTableViewDelegate.h \
    src/ui/SessCreateForm.h \
    src/ui/MainWindow.h \
    src/ui/FMgr.h \
    src/logger.h \
    src/net/put_channel.h

FORMS += \
    src/ui/SessCreateForm.ui \
    src/ui/MainWindow.ui \
    src/ui/FMgr.ui

win32:LIBS += -lws2_32 \
	      -llibssh2

unix:!macx:!symbian: LIBS += -L$$PWD/../../../../../usr/lib/ -lssh2

INCLUDEPATH += $$PWD/../../../../../usr/include
DEPENDPATH += $$PWD/../../../../../usr/include
