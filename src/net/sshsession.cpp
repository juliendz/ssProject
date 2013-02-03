#include "sshsession.h"

SSHSession::SSHSession( Session* session ) : CHANNELS( 8, NULL ){

    this->hostname = session->hostname;
    this->username = session->username;
    this->password = session->password;
    this->port =  session->port;
    this->running_procs = 0;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(process()));

    this->sock = socket(AF_INET, SOCK_STREAM, 0);

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    if(this->sock == INVALID_SOCKET){
        int errcode = WSAGetLastError();
        WSACleanup();
        return;
    }
#else
    if(this->sock == -1){
       return;
    }
#endif

 //Setting socket I/O mode to non-blocking
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    u_long arg = 1;
    ioctlsocket(this->sock, FIONBIO, &arg);
#else
    int flags = fcntl(this->sock, F_GETFL, 0);
    int a = fcntl(this->sock, F_SETFL, flags | O_NONBLOCK);
    qDebug() << a;
#endif

    this->addr.sin_family = AF_INET;
    this->addr.sin_port = htons(this->port.toUtf8().toUShort());
    this->addr.sin_addr.s_addr = inet_addr(this->hostname.toUtf8().constData());

    this->state = ::SESSION_CLOSED;
}

SSHSession::~SSHSession(){
}


SSH_SESSION_STATE SSHSession::begin_session(){

    /////////////////////////////////////////////
    //Perform the connect call
    if(this->state == ::SESSION_PERFORM_CONNECT || this->state == ::SESSION_CLOSED){

        int ret = ::connect(this->sock, (struct sockaddr*)(&this->addr), sizeof(struct sockaddr_in));

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        int ec = WSAGetLastError();
        if(ret == SOCKET_ERROR &&  ec == WSAEWOULDBLOCK) {
            this->state = ::SESSION_SELECT_SOCKET;
         }
#else
    if(ret == -1 &&  errno == EINPROGRESS) {
           this->state = ::SESSION_SELECT_SOCKET;
    }
#endif
        //Success (will most likely never happen) ?
        else if(ret == 0){
            this->state = ::SESSION_PERFORM_INIT;

        //Major error
        } else{
            this->running_procs--;
            this->state = ::SESSION_OPEN_ERROR;
        }

    /////////////////////////////////////////////
    //Poll the socket after connect()
    }else if(this->state == ::SESSION_SELECT_SOCKET){

        fd_set fd_write, fd_error;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
         TIMEVAL timeout;
#else
        struct timeval timeout;
#endif
        // timeout after 10 seconds
        int timeoutSec = 10;

        FD_ZERO(&fd_write);
        FD_ZERO(&fd_error);
        FD_SET(this->sock, &fd_write);
        FD_SET(this->sock, &fd_error);

        timeout.tv_sec = timeoutSec;
        timeout.tv_usec = 0;


#if defined( WIN32 ) || defined( _WIN32 ) || defined( __WIN32 ) && !defined( __CYGWIN__ )
        int ret = select( 0, NULL, &fd_write, &fd_error, &timeout );
#else
        int ret = select( this->sock + 1, NULL, &fd_write, &fd_error, &timeout );
#endif

        //Timed out ?
        if(ret == 0) {
            this->state = ::SESSION_OPEN_ERROR;
        }else{
            //socket data ?
            if(FD_ISSET(this->sock, &fd_write)) {
                this->state = ::SESSION_PERFORM_INIT;
                fprintf(stderr, "connect done..\n");
            }
            //socket error
            else if(FD_ISSET(this->sock, &fd_error)) {
                this->state = ::SESSION_OPEN_ERROR;
                this->running_procs--;
                fprintf(stderr, "connect error..\n");
            //Continue polling next pass
            }else{
                this->state = ::SESSION_SELECT_SOCKET;
            }

        }

    /////////////////////////////////////////////
    //Initialize the libssh2 session object
     }else if(this->state  == ::SESSION_PERFORM_INIT){

        //Create a session instance
        this->session = libssh2_session_init();
        if(!this->session){
            this->state = ::SESSION_OPEN_ERROR;
        }

        //Notify libssh2 we are non-blocking
        libssh2_session_set_blocking(this->session, 0);

        //Move on to the next step
        this->state = ::SESSION_PERFORM_HANDSHAKE;

    /////////////////////////////////////////////////////////////////////////////
    //Exchange welcome banners,keys and setup crypto, compression, and MAC layers
    }else if(this->state == ::SESSION_PERFORM_HANDSHAKE){

        rc = libssh2_session_handshake(this->session, this->sock);
        //Blocking ?
        if(rc == LIBSSH2_ERROR_EAGAIN){
            this->state = ::SESSION_PERFORM_HANDSHAKE;
        //Failure?
        }else if(rc){
            this->state = ::SESSION_OPEN_ERROR;
            this->running_procs--;
        }
        //success ?
        else if(rc == 0) {
            this->state = ::SESSION_PERFORM_AUTH;

            //Check the hostkey's fingerprint against know/stored fingerprint
            //const char* fingerprint = libssh2_hostkey_hash(this->session, LIBSSH2_HOSTKEY_HASH_SHA1);

            /*fprintf(stderr, "Fingerprint: ");
            qDebug() << "Fingerprint: ";
            for(i = 0; i < 20; i++) {
                    qDebug() <<  QString((unsigned char)fingerprint[i]);
            }*/

        }

    //////////////////////////
    //Perform authentication
    } else if(this->state == ::SESSION_PERFORM_AUTH){

        //Authenticate via password
        rc = libssh2_userauth_password(this->session, this->username.toUtf8().constData(), this->password.toUtf8().constData()) ;

        //blocking
        if(rc == LIBSSH2_ERROR_EAGAIN){
            this->state = ::SESSION_PERFORM_AUTH;
        }
        //failure?
        else if(rc){
            this->state = ::SESSION_OPEN_ERROR;
            this->running_procs--;
            qDebug() << "auth failed..\n";

        //success ?
        }else if(rc == 0){
            this->state = ::SESSION_OPEN;
            qDebug() << "auth done..\n";
            this->running_procs--;
            qDebug() << this->running_procs;

        }
    }

    return this->state;
}

bool SSHSession::end_session(){

 libssh2_session_disconnect(this->session, "Normal Shutdown, Thank you for playing");

 libssh2_session_free(session);

#ifdef WIN32
    closesocket(this->sock);
#else
    ::close(this->sock);
#endif

return true;

}

void SSHSession::sftp_ls(QString path){

    int prev_running_procs = this->running_procs;

    LS_Channel *channel = (LS_Channel*)this->CHANNELS[0];

    //Is it instantiated ?
    if(!channel){
        channel = new LS_Channel(this);
        channel->state = ::CHANNEL_OPENING;

    //Is it closed ?
    }else if(channel->state == ::CHANNEL_CLOSED){
        channel->state = ::CHANNEL_OPENING;
    }
    //Is it idle ?
    else if(channel->state == ::CHANNEL_IDLE){
        channel->state = ::CHANNEL_OPERATION_INPROGRESS;
    }

    //Set the request path
    channel->m_path = path;
    channel->m_PATHS = NULL;
    this->CHANNELS[0] = channel;
    this->running_procs++;

    if(this->state == ::SESSION_CLOSED){
        this->state = ::SESSION_PERFORM_CONNECT;
        this->running_procs++;
    }

    //Call process() only if no previously running processes
    if(prev_running_procs <= 0){
        this->timer->start(0);
        fprintf(stderr, "TIMER STARTED\n");
    }

}

void SSHSession::sftp_get_ls(exNodeList *FILES, QString remCurrPath ) {

    int prev_running_procs = this->running_procs;

    LS_Channel *channel = (LS_Channel*)this->CHANNELS[0];

    //Is it instantiated ?
    if(!channel){
        channel = new LS_Channel(this);
        channel->state = ::CHANNEL_OPENING;

    //Is it closed ?
    }else if(channel->state == ::CHANNEL_CLOSED){
        channel->state = ::CHANNEL_OPENING;
    }
    //Is it idle ?
    else if(channel->state == ::CHANNEL_IDLE){
        channel->state = ::CHANNEL_OPERATION_INPROGRESS;
    }

    //Set the request path
    channel->LISTING_Ex = FILES;
    channel->rootPath = remCurrPath;
    channel->list_counter = 0;
    channel->m_path = "";

    this->CHANNELS[0] = channel;
    this->running_procs++;

    //Call process() only if no previously running processes
    if(prev_running_procs <= 0){
        this->timer->start(0);
        //fprintf(stderr, "TIMER STARTED\n");
    }

}

void SSHSession::sftp_put_ls( exNodeList *FILES ){
        int prev_running_procs = this->running_procs;
        LS_Channel *channel = ( LS_Channel* )this->CHANNELS[ 0 ];
        if ( !channel ) {										//Is it instantiated ?
                channel = new LS_Channel(this);
                channel->state = ::CHANNEL_OPENING;
        } else if ( channel->state == ::CHANNEL_CLOSED ) { 			//Is it closed ?
                channel->state = ::CHANNEL_OPENING;
        } else if ( channel->state == ::CHANNEL_IDLE ) {     			//Is it idle ?
                channel->state = ::CHANNEL_OPERATION_INPROGRESS;
        }
        //Set the request path
        channel->LISTING_Ex = FILES;
        channel->list_counter = 0;
        channel->optype = ::PUT_LS;
        channel->m_path = "";
        this->CHANNELS[0] = channel;
        this->running_procs++;
        if( prev_running_procs <= 0 ) {							//Call process() only if no previously running processes
                this->timer->start(0);
                fprintf(stderr, "TIMER STARTED\n");
        }
}


void SSHSession::sftp_get(exNode *file, QString currentLocalPath){

    int prev_running_procs = this->running_procs;
    GET_Channel *channel = 0;
    int channel_index = 2;

    //2-5 to be used for downloads
    for(int i=2; i < 6; i++){
        if ( this->CHANNELS[i] ) {
                if( this->CHANNELS[i]->state == ::CHANNEL_IDLE || this->CHANNELS[i]->state == ::CHANNEL_OPERATION_DONE ){
                        channel = (GET_Channel*)this->CHANNELS[i];
                        channel->state = ::CHANNEL_OPERATION_INPROGRESS;
                        channel_index = i;
                        break;
                }else if ( this->CHANNELS[i]->state == ::CHANNEL_CLOSED ){    		//Is it closed ?
                        channel = (GET_Channel*)this->CHANNELS[i];
                        channel->state = ::CHANNEL_OPENING;
                        channel_index = i;
                        break;
                }
                channel_index++;
        }
    }

    if(!channel){													//Not yet found an instantiated channel?
        channel = new GET_Channel(this);
        channel->state = ::CHANNEL_OPENING;
    }

    qDebug() << "CHANNE INDEX : "+QString::number(channel_index);


    //Set the request path
    channel->currentNode = file;
    channel->currentLocalPath = currentLocalPath;

    this->CHANNELS[channel_index] = channel;
    this->running_procs++;

    //Call process() only if no previously running processes
    if(prev_running_procs <= 0){
        this->timer->start(0);
    }
    return;
}

void SSHSession::sftp_put( exNode *file, QString currentRemotePath){
        int prev_running_procs = this->running_procs;
        GET_Channel *channel = 0;
        int channel_index;
        for ( int i=6; i<9; i++ ) {								//6-9 to be used for uploads
                if ( !this->CHANNELS[i] || !this->CHANNELS[i]->is_open ) {
			channel = (GET_Channel*)this->CHANNELS[i];
                	channel_index = i;
                    	break;
                }
        }
        if ( !channel ) {										//Is it instantiated ?
        	channel = new GET_Channel(this);
        	channel->state = ::CHANNEL_OPENING;
        }else if ( channel->state == ::CHANNEL_CLOSED ) {			//Is it closed ?
        	channel->state = ::CHANNEL_OPENING;
        } else if ( channel->state == ::CHANNEL_IDLE ) {				//Is it idle ?
        	channel->state = ::CHANNEL_OPERATION_INPROGRESS;
    	}
        channel->currentNode = file;							//Set the request path
        channel->currentLocalPath = currentRemotePath;
        this->CHANNELS[channel_index] = channel;
        this->running_procs++;
        if(prev_running_procs <= 0){							//Call process() only if no previously running processes
            this->timer->start(0);
            fprintf(stderr, "TIMER STARTED\n");
        }
}

LIBSSH2_SESSION *SSHSession::getSessionObject() { return this->session; }

void SSHSession::process(){

    if(this->state != ::SESSION_OPEN) {
        this->begin_session();
    }else{

         QVectorIterator <ISSHChannel*> i(this->CHANNELS);
     int index = 0;

         //Process each channel
         while(i.hasNext()){

             ISSHChannel *channel = i.next();

            //Is it instantiated ?
            if(channel){

                if(channel->state == ::CHANNEL_IDLE){
                    continue;

                }else if(channel->state == ::CHANNEL_OPENING){
                    channel->open_channel();

                }else if(channel->state == ::CHANNEL_OPEN){
                    channel->perform_operation();

                }else if(channel->state == ::CHANNEL_OPEN_ERROR){
                    this->running_procs--;

                }else if(channel->state == ::CHANNEL_OPERATION_INPROGRESS){
                    channel->perform_operation();

                } else if(channel->state == ::CHANNEL_OPERATION_DONE){
                    this->running_procs--;
                    channel->state = ::CHANNEL_IDLE;

                }else if(channel->state == ::CHANNEL_OPEN_ERROR){
                    this->running_procs--;
                }
            }
        index++;
        }
    }

    if(this->running_procs <= 0){
        this->timer->stop();
        //fprintf(stderr, "TIMER STOPPED\n");
    }


}


//Wrapper function to emit signal
void SSHSession::emit_receivedFileListing(QList<Node *> *LISTING){
    emit this->sg_lsReady( LISTING );
}

//Wrapper function to emit signal
void SSHSession::emit_getQueueReady( exNodeList *LISTING ){
        emit this->sg_getQueueReady( LISTING );
}

//Wrapper function to emit signal
void SSHSession::emit_readyToSendFileListing(exNodeList *LISTING ) {
    emit this->readyToSendFileListing( LISTING );
}

//Wrapper function to emit signal
void SSHSession::emit_progressUpdate ( exNode* node, int percent_perc ) {
    emit this->sg_progressUpdate( node, percent_perc ) ;
}

//Wrapper function to emit signal
void SSHSession::emit_getDone ( exNode* node, int percent_perc ) {
    emit this->sg_getDone( node, percent_perc ) ;
}
