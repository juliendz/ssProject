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
#include "transferstableviewdelegate.h"
#include "../common.h"
#include "../net/sshsession.h"
#include "../sessions/Session.h"
#include <QTimer>
#include <QHash>
#include <QList>
#include <QStandardItem>



class Session;

class FMgr : public QWidget {
    
    Q_OBJECT
public:
    FMgr(Session* session);
    virtual ~FMgr();

public slots:
	//Event Handlers
    void sl_lsReady(QList<Node*> *FILES);
    void sl_getQueueReady( exNodeList *FILES );
        void 				sl_progressUpdate( exNode *node, int progress_perc );
        void 				sl_getDone( exNode *node, int progress_perc );
    void eh_fileReceived();
    void eh_fileUploaded(Node* file);
    void eh_upload_job_prepared(Node* file);
    void eh_logger_update(QByteArray);

    void eh_clicked_upload_loc_ctx_menu();
        void 				sl_triggered_remDownloadAction( );
    void eh_rem_tableViewItemDoubleClicked(const QModelIndex& index);
    void eh_loc_tableViewItemDoubleClicked(const QModelIndex& index);
    void eh_loc_drives_cbox_changed(QString);
    
    void showLocCtxMenu(const QPoint &);
    void showRemCtxMenu(const QPoint& pos);

signals:
        void				sg_ls( QString currPath );
        void				sg_get_ls( exNodeList* NODES, QString remCurPath);
        void				sg_get( exNode* node, QString locCurPath );

private:
    Ui::FMgr widget;
    QMenu* locaCtxlMenu;
    QMenu* remCtxMenu;
    TransfersTableViewDelegate* downloadsDelegate;
    int dlQueueCount;
    int dlQueueIndex;
    int dlQueueCurIndex;
    int dlQueueRunning;

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
    QStandardItemModel* model_transfers_queued;
    QStandardItemModel* model_transfers_failed;
    QStandardItemModel* model_transfers_completed;

    QStringList rem_PATH;
    
    //Queues
    //QQueue<ssFileTreeNode> UPLOADS;
    //QQueue<ssFileTreeNode> DOWNLOADS;
    QList<Node *> 				*FILES;
    exNodeList 					*getQueue;
    exNodeList 					*putQueue;
    exNodeList 					*completedQueue;
    exNodeList 					*failedQueue;
    QHash<exNode*, QStandardItem*>		*view_transfers_model;

    void 			setupTableView();
    void 			setupCtxMenus();
    void addRow();
    
        inline void add_to_rem_path( QString dir ) {
                this->rem_PATH.append( dir );
                this->FILES->clear( );
        }

        inline void  rem_path_one_up( ) {
                if ( !this->rem_PATH.isEmpty( ) ) {
                        this->rem_PATH.removeLast( );
                        this->FILES->clear( );
                }
        }

        inline QString get_curr_rem_path( ) {
                return "/" + this->rem_PATH.join( "/" ) + "/";
        }

        inline QString get_curr_loc_path( ){
                return this->model_local->rootPath( );
        }


    

};

#endif	/* _FMGR_H */
