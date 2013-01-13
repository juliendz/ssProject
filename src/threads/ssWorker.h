/* 
 * File:   ssWorker.h
 * Author: Julien Dcruz
 * Created on August 3, 2012, 08:37 PM
 */

#ifndef SSWORKER_H
#define	SSWORKER_H

#include <QThread>

class ssWorker: public QThread{
	Q_OBJECT
	public:
		ssWorker();
		virtual ~ssWorker();

	private:
		void startThread();

        private slots:
		void stopThread();

};	


#endif	/* SSWORKER_H */


