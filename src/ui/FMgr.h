/* 
 * File:   FMgr.h
 * Author: Aetion-LP
 * Created on May 26, 2012, 9:02 PM
 */

#ifndef _FMGR_H
#define	_FMGR_H

#include "ui_FMgr.h"
#include "../threads/ssWorker.h"
#include <QStandardItemModel>
#include <QThread>
#include <QRegExp>
#include <QFileSystemModel>
#include <QDesktopServices>
#include <QMenu>
#include <QSignalMapper>
#include <QQueue>
#include <QItemSelectionModel>
#include "SessionsTableViewDelegate.h"
#include "../common.h"
#include "../net/sshsession.h"
#include "../sessions/Session.h"
#include <QTimer>



class Session;

class FMgr : public QWidget {
    
    Q_OBJECT
public:
    FMgr(Session* session);
    virtual ~FMgr();

public slots:
	//Event Handlers
    void eh_fileListReceived(QList<Node*> *FILES);
    void eh_fileListReceived_ex(Node_ex_List *FILES);
    void eh_fileReceived();
    void eh_fileUploaded(Node* file);
    void eh_upload_job_prepared(Node* file);
    void eh_logger_update(QByteArray);

    void eh_clicked_upload_loc_ctx_menu();
    void eh_rem_tableViewItemDoubleClicked(const QModelIndex& index);
    void eh_loc_tableViewItemDoubleClicked(const QModelIndex& index);
    void eh_loc_drives_cbox_changed(QString);
    
    void showLocCtxMenu(const QPoint &);
    void showRemCtxMenu(const QPoint& pos);
    
    
private:
    Ui::FMgr widget;
    
    //Session object
    Session* session;
    
    //Underlying session object
    SSHSession* sshsession;
    
    //The worker thread for the network object
    ssWorker* worker;

    //Timer for network operations
    QTimer *timer;

    //Underlying models for the filemanagers
    QFileSystemModel* model_local;
    QStandardItemModel* model_remote;    
    QStandardItemModel* model_uploads;    
    QStandardItemModel* model_downloads;    
    
    QString currUrl;
    QStringList rem_PATH;
    
    //Queues
    //QQueue<ssFileTreeNode> UPLOADS;
    //QQueue<ssFileTreeNode> DOWNLOADS;
    
    void setupTableView();
    void addRow();
    void call_connect();
	void create_worker_thread();
	void destroy_worker_thread();
    
    QString add_to_rem_path(QString path);
    QString rem_from_rem_path();
    QString get_curr_rem_path();
    
    QString get_curr_loc_path();
    //void traverseJobTree(ssNode *fileNode);
    

};

#endif	/* _FMGR_H */
