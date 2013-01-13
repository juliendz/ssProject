/*
 * Application : Smooth-SCP
 * File:   main.cpp
 * Author: Julien Dcruz
 * Created on May 1, 2012, 12:36 PM
 */

#include <QtGui/QApplication>
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    qRegisterMetaType<Node_ex_List*>("Node_ex_List*");
    qRegisterMetaType<Node_ex*>("Node_ex*");

#ifdef WIN32
    WSADATA wsadata;

    int ret = WSAStartup(MAKEWORD(2,0), &wsadata);
    if(ret != NO_ERROR){
    }

#endif

    MainWindow mainWindow;
    mainWindow.showMaximized();


    return app.exec();
}
