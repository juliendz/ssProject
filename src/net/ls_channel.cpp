#include "ls_channel.h"

LS_Channel::LS_Channel(SSHSession *session){

    this->is_open= false;
    this->session = session;

    this->ssh_session = session->getSessionObject();
    this->channel = 0;
    this->handle = 0;

    this->LISTING = new QList<Node*>();

}

SSH_CHANNEL_STATE LS_Channel::open_channel(){

    this->ssh_session = this->session->getSessionObject();

    this->channel = libssh2_sftp_init(this->ssh_session);

    //Blocking ?
    if ((!this->channel) && (libssh2_session_last_errno(this->ssh_session) !=  LIBSSH2_ERROR_EAGAIN)) {
        this->is_open = false;
        this->state = ::CHANNEL_OPEN_ERROR;

    //Success ?
    }else if(this->channel){
        this->is_open = true;
        this->state =  ::CHANNEL_OPEN;

    //Continue polling ?
    }else{
        this->is_open = false;
        this->state =  ::CHANNEL_OPENING;
     }

    return this->state;
}

SSH_CHANNEL_STATE LS_Channel::close_channel(){

    libssh2_sftp_shutdown(this->channel);
    this->state =  ::CHANNEL_CLOSED;
    return this->state;
}

SSH_CHANNEL_STATE LS_Channel::perform_operation(){

    if(this->m_path != ""){

        //Open the directory and get the sftp handle to it
        if(!this->handle){

            this->handle = libssh2_sftp_opendir(this->channel, this->m_path.toUtf8().constData());

            if((!this->handle) && (libssh2_session_last_errno(this->ssh_session) != LIBSSH2_ERROR_EAGAIN)) {
                        fprintf(stderr, "Unable to open dir with SFTP\n");
                        this->state = ::CHANNEL_OPERATION_ERROR;
                        return this->state;
             }

            if(!this->handle){
                this->state = ::CHANNEL_OPERATION_INPROGRESS;
                return this->state;
            }
        }

        //Parse the directory listing
        int rc = 0;
        char mem[512];
        LIBSSH2_SFTP_ATTRIBUTES file_attrs;

        //Read a 512 byte block of data
        rc = libssh2_sftp_readdir(this->handle, mem, sizeof(mem),  &file_attrs);

        if(rc > 0){

            Node* node = new Node();

            //PERMISSIONS and TYPE
            if(file_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) {
                node->perms = "perms";

               //regular file ?
               if(LIBSSH2_SFTP_S_ISREG(file_attrs.permissions)){
                    node->type = 0;
               //folder ?
               }else if(LIBSSH2_SFTP_S_ISDIR(file_attrs.permissions)){
                    node->type = 1;
               //Symbolic link ?
               }else if(LIBSSH2_SFTP_S_ISLNK(file_attrs.permissions)){
                    node->type = 2;
               }

             }else{
                node->perms = "n/a";
             }

            //ACCESS AND MODIFIED TIME
            if(file_attrs.flags & LIBSSH2_SFTP_ATTR_ACMODTIME) {

                node->atime = QDateTime::fromTime_t(file_attrs.atime).toString("MM/dd/yyyy h:m:s ap");
                node->mtime = QDateTime::fromTime_t(file_attrs.mtime).toString("MM/dd/yyyy h:m:s ap");

             }else{
                node->atime = "n/a";
                node->mtime = "n/a";
             }

            //OWNERS and GROUPS
            if(file_attrs.flags & LIBSSH2_SFTP_ATTR_UIDGID) {

                node->uid = file_attrs.uid;
                node->gid = file_attrs.gid;
            } else {
                node->uid = 999;
                node->gid = 999;
            }

            //SIZE
            if(file_attrs.flags & LIBSSH2_SFTP_ATTR_SIZE) {
                node->size = file_attrs.filesize;
            }

            //NAME
            node->name = QString(mem);
            //PATH
            node->absPath = this->m_path +  node->name ;

            this->LISTING->append(node);


        }else if(rc == LIBSSH2_ERROR_EAGAIN) {

            this->state = ::CHANNEL_OPERATION_INPROGRESS;

        }else{

            //Close the handle
            libssh2_sftp_closedir(this->handle);
            this->handle = NULL;

            //Pass the listing to the SSH object for emiting
            this->session->emit_receivedFileListing(this->LISTING);
            this->state = ::CHANNEL_OPERATION_DONE;

        }
/////////////////////////////////////////////////////////////////////////////////////////////////////
    }else{

        //Open the directory and get the sftp handle to it
        if(!this->handle){

            //Update the current path to list
            Node_ex* temp_node = this->LISTING_Ex->at(this->list_counter);
            if(temp_node->type == 1){
                this->currentPath = temp_node->absPath;
            }else{
                this->list_counter++;
                this->state = ::CHANNEL_OPERATION_INPROGRESS;
                return this->state;
            }



            this->handle = libssh2_sftp_opendir(this->channel, this->currentPath.toUtf8().constData());

            if((!this->handle) && (libssh2_session_last_errno(this->ssh_session) != LIBSSH2_ERROR_EAGAIN)) {
                        fprintf(stderr, "Unable to open dir with SFTP\n");
                        this->state = ::CHANNEL_OPERATION_ERROR;
                        return this->state;
             }

            if(!this->handle){
                this->state = ::CHANNEL_OPERATION_INPROGRESS;
                return this->state;
            }
        }


        //Parse the directory listing
        int rc = 0;
        char mem[512];
        LIBSSH2_SFTP_ATTRIBUTES file_attrs;


        //Read a 512 byte block of data
        rc = libssh2_sftp_readdir(this->handle, mem, sizeof(mem),  &file_attrs);

        if(rc > 0){

            QString name = QString(mem);

            if(name != ".." && name != "."){

                Node_ex* node = new Node_ex();

                //PERMISSIONS and TYPE
                if(file_attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) {

                   //regular file ?
                   if(LIBSSH2_SFTP_S_ISREG(file_attrs.permissions)){
                      node->type = 0;
                   //folder ?
                   }else if(LIBSSH2_SFTP_S_ISDIR(file_attrs.permissions)){
                      node->type = 1;
                   //Symbolic link ?
                   }else if(LIBSSH2_SFTP_S_ISLNK(file_attrs.permissions)){
                      node->type = 2;
                   }

                 }else{
                 }

                //SIZE
                if(file_attrs.flags & LIBSSH2_SFTP_ATTR_SIZE) {
                    node->size = file_attrs.filesize;
                }

                //PATH
                node->name = name;
                node->absPath = currentPath + "/" + name ;

                this->LISTING_Ex->insert(this->list_counter+1, node);
          }


        }else if(rc == LIBSSH2_ERROR_EAGAIN) {

            this->state = ::CHANNEL_OPERATION_INPROGRESS;

        }else{

            //Close the handle
            libssh2_sftp_closedir(this->handle);
            this->handle = NULL;
            this->list_counter++;

            if(this->list_counter < this->LISTING_Ex->count()){
                this->state = ::CHANNEL_OPERATION_INPROGRESS;
            }else{
                this->state = ::CHANNEL_OPERATION_DONE;
                this->session->emit_receivedFileListing_ex(this->LISTING_Ex);
            }

        }


    }


    return this->state;

}
