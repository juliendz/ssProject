/* 
 * Application : Smooth-SCP
 * File:   Session.h
 * Author: Julien
 * Created on May 3, 2012, 9:36 PM
 */

#ifndef SESSION_H
#define	SESSION_H

#include <QString>
#include <QMap>

class Session {

public:
    Session(QMap<QString, QString> DETAILS);
    virtual ~Session();

    QString id;
    QString name;
    QString hostname;
    QString username;
    QString password;
    QString port;
    QString type;
    QString initpath;

private:

};

#endif	/* SESSION_H */

