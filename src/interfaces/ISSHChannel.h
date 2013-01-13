#ifndef SSHCHANNEL_H
#define SSHCHANNEL_H

#include <QObject>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include "../common.h"

class ISSHChannel : public QObject {
    Q_OBJECT
public:
    SSH_CHANNEL_STATE state;
    
public:
    virtual  SSH_CHANNEL_STATE open_channel() = 0;
    virtual SSH_CHANNEL_STATE close_channel() = 0;
    virtual SSH_CHANNEL_STATE perform_operation() = 0;

public:
    bool is_open;

};


#endif // SSHCHANNEL_H
