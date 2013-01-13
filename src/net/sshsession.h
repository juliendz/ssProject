#ifndef SSHSESSION_H
#define SSHSESSION_H

#include <QCoreApplication>
#include <QObject>
#include <QVector>
#include <QThread>
#include "../interfaces/ISSHChannel.h"
#include "../common.h"
#include <stdio.h>
#include <QTimer>
#include <QVectorIterator>
#include "../sessions/Session.h"


#include <libssh2.h>
#include <libssh2_sftp.h>

#include "ls_channel.h"
#include "get_channel.h"

#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif


/* last resort for systems not defining PRIu64 in inttypes.h */
#ifndef __PRI64_PREFIX
#ifdef WIN32
#define __PRI64_PREFIX "I64"
#else
#if __WORDSIZE == 64
#define __PRI64_PREFIX "l"
#else
#define __PRI64_PREFIX "ll"
#endif
#endif
#endif
#ifndef PRIu64
#define PRIu64 __PRI64_PREFIX "u"
#endif

class SSHSession : public QObject {
    Q_OBJECT
public:
    explicit SSHSession(Session *session);
    virtual ~SSHSession();

signals:
    
public slots:
    SSH_SESSION_STATE begin_session();
    bool end_session();
    void process();
    void sftp_ls(QString path);
    void sftp_ls_ex(Node_ex_List *FILES);
    void sftp_get(Node_ex* file, QString currentPath);

public:
    LIBSSH2_SESSION* getSessionObject();
    void emit_receivedFileListing(QList<Node*>* LISTING);
    void emit_receivedFileListing_ex(Node_ex_List *LISTING);

signals:
    void receivedFileListing(QList<Node*>* LISTING);
    void receivedFileListing_ex(Node_ex_List *LISTING);

private:
    QTimer *timer;
    LIBSSH2_SESSION *session;
    QVector<ISSHChannel*> CHANNELS;

    QString hostname;
    QString username;
    QString password;
    QString port;
    QString host_fingerprint;

    int sock;
    int i;
    struct sockaddr_in addr;
    int rc;

    int running_procs;

    SSH_SESSION_STATE state;
    
};

#endif // SSHSESSION_H
