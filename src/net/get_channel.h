#ifndef GET_CHANNEL_H
#define GET_CHANNEL_H

#include <QObject>
#include <QDir>
#include <QFile>
#include "sshsession.h"
#include "../common.h"
#include <QDebug>

class SSHSession;

class GET_Channel : public ISSHChannel
{
    Q_OBJECT
public:
    explicit GET_Channel(SSHSession *session = 0);
    ~GET_Channel();

public:
    SSH_CHANNEL_STATE open_channel();
    SSH_CHANNEL_STATE close_channel();
    SSH_CHANNEL_STATE perform_operation();
    exNode* currentNode;
    QString currentLocalPath;
    

private:
    SSHSession* session;
    LIBSSH2_SESSION *ssh_session;
    LIBSSH2_SFTP *channel;
    LIBSSH2_SFTP_HANDLE *handle;
    QFile* file;

    
public slots:
    
};

#endif // GET_CHANNEL_H
