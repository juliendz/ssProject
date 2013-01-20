#ifndef SFTPCHANNEL_H
#define SFTPCHANNEL_H

#include <QObject>
#include <QList>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include "../interfaces/ISSHChannel.h"
#include "sshsession.h"
#include "../common.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QFileInfo>

class SSHSession;


class LS_Channel : public ISSHChannel {
    Q_OBJECT
public:
    explicit LS_Channel( SSHSession* session );
public:
    SSH_CHANNEL_STATE 		open_channel();
    SSH_CHANNEL_STATE 		close_channel();
    SSH_CHANNEL_STATE 		perform_operation();

    QString				        m_path;
    QList<QString>			*m_PATHS;
    int 					list_counter;
    QString 					currentPath;
    QList<Node*>			*LISTING;
    Node_ex_List 				*LISTING_Ex;
    SSH_LS_OPERATION		optype;
private:
    void put_ls();

    SSHSession				*session;
    LIBSSH2_SESSION 			*ssh_session;
    LIBSSH2_SFTP 			*channel;
    LIBSSH2_SFTP_HANDLE 		*handle;


};

#endif // SFTPCHANNEL_H
