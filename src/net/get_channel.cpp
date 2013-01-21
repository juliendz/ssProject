#include "get_channel.h"

GET_Channel::GET_Channel(SSHSession *session){

    this->is_open= false;
    this->session = session;

    this->ssh_session = session->getSessionObject();
    this->channel = 0;
    this->handle = 0;

    this->file = new QFile();


}

GET_Channel::~GET_Channel(){
    delete this->file;
}

SSH_CHANNEL_STATE GET_Channel::open_channel(){

    this->channel = libssh2_sftp_init(this->ssh_session);

    if ((!this->channel) && (libssh2_session_last_errno(this->ssh_session) !=  LIBSSH2_ERROR_EAGAIN)) {
        this->is_open = false;
        this->state = ::CHANNEL_OPEN_ERROR;
    }else if(this->channel){
        this->is_open = true;
        this->state =  ::CHANNEL_OPEN;
    }else{
        this->is_open = false;
        this->state =  ::CHANNEL_OPENING;
     }

    return this->state;
}

SSH_CHANNEL_STATE GET_Channel::close_channel(){

    libssh2_sftp_shutdown(this->channel);
    this->state =  ::CHANNEL_CLOSED;
    return this->state;
}



SSH_CHANNEL_STATE GET_Channel::perform_operation(){
    if(this->currentNode->type == 1){//If its a folder create it locally
        QDir dir(this->currentLocalPath + "/" + this->currentNode->name);
        if(!dir.exists()){
            dir.mkdir(this->currentLocalPath + "/" + this->currentNode->name);
            this->state = ::CHANNEL_OPERATION_DONE;
        }
    }else{

        //Open the file via FTP
        if(!this->handle){
              this->handle = libssh2_sftp_open(this->channel, this->currentNode->absPath.toUtf8().constData(),  LIBSSH2_FXF_READ, 0);

              if (!this->handle && libssh2_session_last_errno(this->ssh_session) != LIBSSH2_ERROR_EAGAIN) {
                     this->state = ::CHANNEL_OPERATION_ERROR;
                    return this->state;
              }

             if(!this->handle){
                    this->state = ::CHANNEL_OPERATION_INPROGRESS;
                    return this->state;
             }else{

                this->file->setFileName(this->currentNode->name);
                QDir::setCurrent(this->currentLocalPath);
                this->file->open(QIODevice::WriteOnly | QIODevice::Append);
             }

        }

        int rc = 0;
        char mem[1024*24];

        rc = libssh2_sftp_read(this->handle, mem,  sizeof(mem));

        if(rc > 0){
            //Write to file
            this->file->write(mem, rc);

        }else if(rc == LIBSSH2_ERROR_EAGAIN){
            this->state = ::CHANNEL_OPERATION_INPROGRESS;
        }else{
            //Done
            this->file->close();
            delete this->currentNode;
            this->state = ::CHANNEL_OPERATION_DONE;
        }
   }

    return this->state;

}
