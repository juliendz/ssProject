/*
 * File:   put_channel.h
 * Author: Julien Dcruz
 * Created on Jan 19, 2013, 20:55 PM
 */

#ifndef PUT_CHANNEL_H
#define PUT_CHANNEL_H

#include <QObject>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include "../interfaces/ISSHChannel.h"

class SSHSession;

class PUT_Channel : public ISSHChannel {
    Q_OBJECT
public:
    explicit PUT_Channel ( SSHSession *session );
    ~PUT_Channel( );

    SSH_CHANNEL_STATE 	open_channel ();
    SSH_CHANNEL_STATE 	close_channel ();
    SSH_CHANNEL_STATE 	perform_operation ();
    exNode* 			currentNode;
    QString 				currentRemotePath;


private:
    SSHSession 			*session;
    LIBSSH2_SESSION 		*ssh_session;
    LIBSSH2_SFTP 		*channel;
    LIBSSH2_SFTP_HANDLE 	*handle;
};
#endif // PUT_CHANNEL_H
