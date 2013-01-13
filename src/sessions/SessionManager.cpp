/* 
 * File:   SessionManager.cpp
 * Author: Julien
 * Created on July 21, 2012, 12:09 PM
 */

#include "SessionManager.h"


bool SessionManager::instanceFlag = false;
SessionManager* SessionManager::instance = 0;

SessionManager::SessionManager() {
    
    //Create the db if not there
    this->create();
    this->init_sess_db_model();    
    
}

void SessionManager::init_sess_db_model(){
    
    //Read the database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("sess.db");
    if (db.open()){
        model = new QSqlTableModel(0, db);
        model->setTable("sessions");
        model->select();
    }
}


SessionManager::~SessionManager() {
     instanceFlag = false;
     delete model;
}

SessionManager* SessionManager::getInstance(){
    if(!instanceFlag){
	instance = new SessionManager();
	instanceFlag = true;
	return instance;
    }else{
	return instance;
    }
}

//Creates the database for session storage
bool SessionManager::create(){
    
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("sess.db");
    
    if(db.open()){
        //Is the session table there ?
        if(db.tables().indexOf("sessions") == -1){
            QSqlQuery query(db);
            query.prepare("CREATE TABLE sessions "
                          "(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "name TEXT,"
                          "hostname TEXT,"
                          "username TEXT,"
                          "password TEXT,"
                          "port INTEGER,"
                          "type TEXT,"
                          "initpath TEXT)");
            if(!query.exec()){
                qDebug() << query.lastInsertId();
                return false;
            }
        }
    }else{
        return false;
    }
    
    db.close();
    return true;
}

//Save a session in the database
bool SessionManager::save(QMap<QString, QString>& DETAILS){
    
    QString session_name = DETAILS.value("name");
    QString hostname = DETAILS.value("hostname");    
    QString username = DETAILS.value("username");
    QString password = DETAILS.value("password");
    QString port = DETAILS.value("port");
    QString type = DETAILS.value("type");
    QString initpath = DETAILS.value("initpath");

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("sess.db");
    
    if(db.open()){
         //Is the session table there ?
        if(db.tables().indexOf("sessions") == -1){
            this->create();            
        }
        
        QSqlQuery query(db);
        query.prepare("INSERT INTO sessions VALUES (NULL, '"+session_name+
                                                   "', '"+hostname +
                                                   "', '"+username+
                                                   "', '"+password+
                                                   "', "+port+
                                                   ", '"+type+
                                                   "', '"+initpath+
                                                   "')");
         if(!query.exec()){
                qCritical() << query.lastError().text();
                return false;
         }
    }else{
        return false;
    }
    
    db.close();
    return true;
}

//load a session in the database
bool SessionManager::load(QString id, QMap<QString, QString>& SESS){
    
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("sess.db");
    
    if(db.open()){
         //Is the session table there ?
        if(db.tables().indexOf("sessions") == -1){
            return false;
        }
        
        QSqlQuery query(db);
        query.prepare("SELECT * from sessions WHERE id="+id);
         if(!query.exec()){
                return false;
         }
         else{
             while (query.next()){
                 QSqlRecord rec = query.record();
                 SESS.insert("id", rec.field("id").value().toString());
                 SESS.insert("name", rec.field("name").value().toString());
                 SESS.insert("hostname", rec.field("hostname").value().toString());
                 SESS.insert("username", rec.field("username").value().toString());
                 SESS.insert("password", rec.field("password").value().toString());
                 SESS.insert("port", rec.field("port").value().toString());
                 SESS.insert("type", rec.field("type").value().toString());
                 SESS.insert("initpath", rec.field("initpath").value().toString());
             }
         }
    }else{
        return false;
    }
    return true;
}

//Creates a session object with data from the db
Session* SessionManager::loadSession(const QModelIndex& index){
    
    //Get the record row from the db table
     QSqlRecord rec = SessionManager::getInstance()->model->record(index.row());
     QMap<QString, QString> SESS;
     SESS.insert("id", rec.field("id").value().toString());
     SESS.insert("name", rec.field("name").value().toString());
     SESS.insert("hostname", rec.field("hostname").value().toString());
     SESS.insert("username", rec.field("username").value().toString());
     SESS.insert("password", rec.field("password").value().toString());
     SESS.insert("port", rec.field("port").value().toString());
     SESS.insert("type", rec.field("type").value().toString());
     SESS.insert("initpath", rec.field("initpath").value().toString());
     return new Session(SESS);
    
}


