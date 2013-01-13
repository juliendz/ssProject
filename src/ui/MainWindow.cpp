/*
 * File:   MainWindow.cpp
 * Author: Aetion-LP
 * Created on May 1, 2012, 1:14 PM
 */

#include "MainWindow.h"

MainWindow::MainWindow() {
    widget.setupUi(this);

    //Remove the id column so the next column is shown and set the model
    SessionManager::getInstance()->model->removeColumn(0);
    widget.listView_sessions->setModel(SessionManager::getInstance()->model);

    //Connect signals to slots
    connect(this->widget.btn_addsess, SIGNAL(clicked()), this, SLOT(showCreateSess()));
    connect(this->widget.listView_sessions, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(startSession(const QModelIndex&)));

}

MainWindow::~MainWindow() {
    //delete colDelegate;
}

void MainWindow::showCreateSess(){
    SessCreateForm* form = new SessCreateForm();
    form->show();
}       

void MainWindow::startSession(const QModelIndex & index){
    
    //Create the session
    Session* sess = SessionManager::getInstance()->loadSession(index);
    
    //Create the tab
    FMgr* fmgr = new FMgr(sess);
    this->widget.tabWidget_Main->addTab(fmgr, sess->name);
}

