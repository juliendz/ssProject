/*
 * File:   SessCreateForm.cpp
 * Author: Julien
 *
 * Created on July 21, 2012, 9:02 PM
 */

#include "SessCreateForm.h"

SessCreateForm::SessCreateForm() {
    widget.setupUi(this);
    
    //Connect signals to slots
    connect(this->widget.btn_create, SIGNAL(clicked()), this, SLOT(saveSession()));
}

SessCreateForm::~SessCreateForm() {
}

void SessCreateForm::saveSession(){
    
    //Validation
    if(widget.txtbox_sessname->text().isEmpty()){
        
    }
    if(widget.txtbox_hostname->text().isEmpty()){
        
    }
    if(widget.txtbox_username->text().isEmpty()){
        
    }
    if(widget.txtbox_password->text().isEmpty()){
        
    }
    if(widget.txtbox_port->text().isEmpty()){
        
    }
    
    QMap<QString, QString> SESS;
    SESS.insert("name", widget.txtbox_sessname->text());
    SESS.insert("hostname", widget.txtbox_hostname->text());
    SESS.insert("username", widget.txtbox_username->text());
    SESS.insert("password", widget.txtbox_password->text());
    SESS.insert("port", widget.txtbox_port->text());
    SESS.insert("type", widget.cbox_type->currentText());
    SESS.insert("initpath", "/");
    //SESS.insert("initpath", widget.txtbox_initpath->currentText());
    SessionManager::getInstance()->save(SESS);
    
    this->close();
    
}
