/* 
 * File:   MainWindow.h
 * Author: Aetion-LP
 * Created on May 1, 2012, 1:14 PM
 */

#ifndef _MAINWINDOW_H
#define	_MAINWINDOW_H

#include "ui_MainWindow.h"
#include <QMap>
#include <QMessageBox>
#include <QDebug>
#include "../sessions/Session.h"
#include "SessCreateForm.h"
#include "FMgr.h"
#include "transferstableviewdelegate.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

  public slots:
      void showCreateSess();
        void startSession(const QModelIndex & index);


public:
    MainWindow();
    virtual ~MainWindow();
private:
    Ui::MainWindow widget;

 
  };

#endif	/* _MAINWINDOW_H */
