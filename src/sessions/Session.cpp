/* 
 * Application : Smooth-SCP
 * File:   Session.cpp
 * Author: Julien Dcruz
 * Created on May 3, 2012, 9:36 PM
 */

#include "Session.h"

Session::Session(QMap<QString, QString> DETAILS) {
    
    this->id = DETAILS["id"];
    this->name = DETAILS["name"];
    this->hostname = DETAILS["hostname"];
    this->username = DETAILS["username"];
    this->password = DETAILS["password"];
    this->port = DETAILS["port"];
    this->type = DETAILS["type"];
    this->initpath = DETAILS["initpath"];
}

Session::~Session() {
}



