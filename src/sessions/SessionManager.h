/* 
 * File:   SessionManager.h
 * Author: Julien
 * Created on July 21, 2012, 12:09 PM
 */

#ifndef SESSIONMANAGER_H
#define	SESSIONMANAGER_H

#include <QList>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QModelIndex>
#include <QSqlTableModel>
#include <QDebug>
#include "Session.h"
#include <QSqlError>

class SessionManager {
    
public:
    static SessionManager* getInstance();
    virtual ~SessionManager();
   
    QSqlTableModel* model;
    
    bool create();
    bool save(QMap<QString, QString>& DETAILS);
    bool load(QString id, QMap<QString, QString>& SESS);
    Session* loadSession(const QModelIndex&);
    
private:
    static bool instanceFlag;
    static SessionManager* instance;
    SessionManager();
    
    void init_sess_db_model();

};

#endif	/* SESSIONMANAGER_H */

