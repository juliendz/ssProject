#ifndef SSHSESSION_H
#define SSHSESSION_H

#include <QCoreApplication>
#include <QObject>
#include <QVector>
#include <QThread>
#include <QTimer>
#include <QVectorIterator>
#include <stdio.h>



#include <libssh2.h>
#include <libssh2_sftp.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
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

#include "../interfaces/ISSHChannel.h"
#include "../common.h"
#include "../sessions/Session.h"
#include "ls_channel.h"
#include "get_channel.h"


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
