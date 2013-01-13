#ifndef LOGGER_H
#define LOGGER_H

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

class Logger
{
public:
    Logger(QString log_name);
    void Log(QString msg);
    QFile log;

};

#endif // LOGGER_H
