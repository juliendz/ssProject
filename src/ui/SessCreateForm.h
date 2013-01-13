/* 
 * File:   SessCreateForm.h
 * Author: Julien
 * Created on July 21, 2012, 9:02 PM
 */

#ifndef _SESSCREATEFORM_H
#define	_SESSCREATEFORM_H

#include "ui_SessCreateForm.h"
#include "../Sessions/SessionManager.h"

class SessCreateForm : public QWidget {
    
    Q_OBJECT
public:
    
    SessCreateForm();
    virtual ~SessCreateForm();
    
public slots:
        void saveSession();
    
private:
    Ui::SessCreateForm widget;
};

#endif	/* _SESSCREATEFORM_H */
