#include "logger.h"

Logger::Logger(QString log_name){

    //this->log = QFile(QDir::currentPath() + "/"+ log_name +".log");

}

void Logger::Log(QString msg){

    /*QTime time = QTime::currentTime();

    this->log.open(QIoDevice::WriteOnly);

    QTextStream ts(&this->log(););
    ts << "[" + time.toString() + "]" + msg << endl;

    this->log.close();
*/
}
