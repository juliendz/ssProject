/*
 * File:   FMgr.cpp
 * Author: Aetion-LP
 * Created on May 26, 2012, 9:02 PM
 */

#include "FMgr.h"
#include <QMessageBox>

FMgr::FMgr( Session* sess ) {
        widget.setupUi(this);
	this->widget.tabWidget_Main->setCurrentIndex(0);

        this->session = sess;
        this->rem_PATH = sess->initpath.split( "/" , QString::SkipEmptyParts );
        this->widget.txtBox_remotepath->setText(this->get_curr_rem_path());

        this->setupTableView();
        this->setupCtxMenus ();
        this->getQueue = new exNodeList( );
        this->completedQueue = new exNodeList( );
        this->failedQueue = new exNodeList( );
        this->view_transfers_model = new QHash<exNode*, QStandardItem*>( );
        this->dlQueueCount = 0;
        this->dlQueueIndex = 0;
        this->dlQueueCurIndex = 0;
        this->dlQueueRunning = 0;

        //tableview item dclick signal/slot
        connect(this->widget.tableView_local, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(eh_loc_tableViewItemDoubleClicked(const QModelIndex&)));
        connect(this->widget.tableView_local, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showLocCtxMenu(const QPoint &)));

        connect(this->widget.tableView_remote, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(eh_rem_tableViewItemDoubleClicked(const QModelIndex&)));
        connect(this->widget.tableView_remote, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showRemCtxMenu(const QPoint &)));

        connect(this->widget.comboBox_drives, SIGNAL(currentIndexChanged(QString)), this, SLOT(eh_loc_drives_cbox_changed(QString)));


        this->sshsession = new SSHSession( this->session );	//Create the ssh session object
        connect( this, SIGNAL( sg_ls( QString ) ), this->sshsession, SLOT( sftp_ls( QString ) ) );
        connect( this->sshsession, SIGNAL( sg_lsReady( QList<Node*>* ) ), this, SLOT( sl_lsReady( QList<Node*>* ) ) );
        connect( this, SIGNAL( sg_get_ls( exNodeList*, QString ) ), this->sshsession, SLOT( sftp_get_ls( exNodeList*, QString ) ) );
        connect( this->sshsession, SIGNAL( sg_getQueueReady( exNodeList* ) ), this, SLOT( sl_getQueueReady( exNodeList* ) ) );
        connect( this, SIGNAL( sg_get( exNode*, QString ) ), this->sshsession, SLOT( sftp_get( exNode*, QString ) ) );
        connect( this->sshsession, SIGNAL( sg_progressUpdate( exNode*, int ) ), this, SLOT( sl_progressUpdate( exNode*, int ) ) );
        connect( this->sshsession, SIGNAL( sg_getDone( exNode*,int ) ), this, SLOT( sl_getDone( exNode*, int ) ) );


        emit this->sg_ls ( this->get_curr_rem_path ( ) );
}


FMgr::~FMgr() {
    delete this->session;
    delete this->sshsession;
}

/////////////////////////////////////////////////////////////////////
//	Set up the file/folder views 
/////////////////////////////////////////////////////////////////////
void FMgr::setupTableView(){
    
    //Set the model for local view
    this->model_local = new QFileSystemModel;
    this->model_local->setFilter(QDir::AllEntries | QDir::Hidden);
    QString defLoc = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
    this->model_local->setRootPath(defLoc);
    this->widget.tableView_local->setModel(this->model_local);
    this->widget.tableView_local->setRootIndex(this->model_local->index(defLoc));
    this->widget.txtBox_localpath->setText(defLoc);
    
    //Setup drives combox box
    QFileInfoList DRIVES = QDir::drives();
    foreach(QFileInfo drive, DRIVES){
        this->widget.comboBox_drives->addItem(drive.absoluteFilePath());
    }
    
    
    //Set the model for remote view
    model_remote = new QStandardItemModel(0,6,this);
    model_remote->setHorizontalHeaderItem(0, new QStandardItem(QString("Filename")));
    model_remote->setHorizontalHeaderItem(1, new QStandardItem(QString("Filesize")));
    model_remote->setHorizontalHeaderItem(2, new QStandardItem(QString("Filetype")));
    model_remote->setHorizontalHeaderItem(3, new QStandardItem(QString("Access Time")));
    model_remote->setHorizontalHeaderItem(4, new QStandardItem(QString("Modified Time")));
    model_remote->setHorizontalHeaderItem(5, new QStandardItem(QString("Permissions")));
    this->widget.tableView_remote->setModel(model_remote);

    //Set the model for queued transfers view
    model_transfers_queued = new QStandardItemModel(0,6,this);
    model_transfers_queued->setHorizontalHeaderItem(0, new QStandardItem(QString("Local File")));
    model_transfers_queued->setHorizontalHeaderItem(1, new QStandardItem(QString("Remote File")));
    model_transfers_queued->setHorizontalHeaderItem(2, new QStandardItem(QString("Direction")));
    model_transfers_queued->setHorizontalHeaderItem(3, new QStandardItem(QString("Size")));
    model_transfers_queued->setHorizontalHeaderItem(4, new QStandardItem(QString("Type")));
    model_transfers_queued->setHorizontalHeaderItem(5, new QStandardItem(QString("Progress")));
    this->widget.tableView_transfers_queued->setModel(model_transfers_queued);


    //Set the model for failed transfers view
    model_transfers_failed = new QStandardItemModel(0,5,this);
    model_transfers_failed->setHorizontalHeaderItem(0, new QStandardItem(QString("Remote File")));
    model_transfers_failed->setHorizontalHeaderItem(1, new QStandardItem(QString("Local File")));
    model_transfers_failed->setHorizontalHeaderItem(2, new QStandardItem(QString("Direction")));
    model_transfers_failed->setHorizontalHeaderItem(3, new QStandardItem(QString("Size")));
    model_transfers_failed->setHorizontalHeaderItem(4, new QStandardItem(QString("Type")));
    model_transfers_failed->setHorizontalHeaderItem(5, new QStandardItem(QString("Progress")));
    this->widget.tableView_transfers_failed->setModel(model_transfers_failed);
    //this->downloadsDelegate = new TransfersTableViewDelegate( this );
    //this->widget.tableView_downloads->setItemDelegate( this->downloadsDelegate  );

    //Set the model for completed transfers view
    model_transfers_completed = new QStandardItemModel(0,5,this);
    model_transfers_completed->setHorizontalHeaderItem(0, new QStandardItem(QString("Remote File")));
    model_transfers_completed->setHorizontalHeaderItem(1, new QStandardItem(QString("Local File")));
    model_transfers_completed->setHorizontalHeaderItem(2, new QStandardItem(QString("Direction")));
    model_transfers_completed->setHorizontalHeaderItem(3, new QStandardItem(QString("Size")));
    model_transfers_completed->setHorizontalHeaderItem(4, new QStandardItem(QString("Type")));
    model_transfers_completed->setHorizontalHeaderItem(5, new QStandardItem(QString("Progress")));
    this->widget.tableView_transfers_completed->setModel(model_transfers_completed);


}

void FMgr::setupCtxMenus( ) {
        this->locaCtxlMenu = new QMenu( this );
        this->remCtxMenu = new QMenu( this );

        QAction* locUploadAction = new QAction( "Upload", this->widget.tableView_local );
        //act->connect(act, SIGNAL(triggered()), this, SLOT(eh_clicked_upload_loc_ctx_menu()));
        this->locaCtxlMenu->addAction ( locUploadAction );
        this->locaCtxlMenu->addSeparator();
        QAction* locOpenAction = new QAction( tr( "Open" ),  this );
        this->locaCtxlMenu->addAction ( locOpenAction );
        QAction* locDeleteAction = new QAction( tr( "Delete" ),  this );
        this->locaCtxlMenu->addAction ( locDeleteAction );
        QAction* locRenameAction = new QAction( tr( "Rename" ),  this );
        this->locaCtxlMenu->addAction ( locRenameAction );
        this->locaCtxlMenu->addSeparator();
        QAction* locCreateDirAction = new QAction( tr( "Create directory" ),  this );
        this->locaCtxlMenu->addAction ( locCreateDirAction );
        QAction* locRefreshAction = new QAction( tr(" Refresh" ),  this );
        this->locaCtxlMenu->addAction ( locRefreshAction );

        QAction* remDownloadAction = new QAction( tr( "Download" ), this );
        remDownloadAction->connect( remDownloadAction , SIGNAL( triggered( ) ), this, SLOT( sl_triggered_remDownloadAction( ) ) );
        this->remCtxMenu->addAction ( remDownloadAction );
        this->remCtxMenu->addSeparator();
        QAction* remOpenAction = new QAction( tr( "Open" ), this );
        this->remCtxMenu->addAction ( remOpenAction );
        QAction* remDeleteAction = new QAction( tr( "Delete" ), this );
        this->remCtxMenu->addAction ( remDeleteAction );
        QAction* remRenameAction = new QAction( tr( "Rename" ), this );
        this->remCtxMenu->addAction ( remRenameAction );
        QAction* remPermsAction = new QAction( tr( "Permissions" ), this );
        this->remCtxMenu->addAction ( remPermsAction );
        QAction* remRefreshAction = new QAction( tr( "Refresh" ), this );
        this->remCtxMenu->addAction ( remRefreshAction );
}

void FMgr::showLocCtxMenu(const QPoint& pos){
        this->locaCtxlMenu->exec(widget.tableView_local->mapToGlobal(pos));
}

void FMgr::showRemCtxMenu(const QPoint& pos){
        this->remCtxMenu->exec(widget.tableView_remote->mapToGlobal(pos));
}

/****************************************************************************************
									Event Handlers 
*****************************************************************************************/

void FMgr::eh_fileUploaded(Node *file){
    
/*    //Remove all data rows
    this->model_remote->removeRows(0, this->model_remote->rowCount());
    
    QMetaObject::invokeMethod(this->connection, "ssGET_FILE_LIST", Q_ARG(QString, this->get_curr_rem_path()));
*/
}

void FMgr::sl_lsReady( QList<Node *> *FILES ) {
        this->FILES = FILES;
        QList<Node*>::iterator iter;
        for ( iter = FILES->begin(); iter != FILES->end(); iter++ ) {    				//Update the model for the view
                QList<QStandardItem*> items;
                items.append( new QStandardItem( QString((*iter)->name) ) );
                items.append( new QStandardItem( QString::number((*iter)->size) ) );
                items.append( new QStandardItem( ( (*iter)->type == 0 ? "File" : ( (*iter)->type == 1 ? "Folder" : "Symlink" ) ) ) );
                items.append( new QStandardItem( (*iter)->atime ) );
                items.append( new QStandardItem( (*iter)->atime) );
                items.append( new QStandardItem( QString( (*iter)->perms) ) );
                this->model_remote->appendRow( items );
        }
}

void FMgr::sl_getQueueReady( exNodeList *FILES ){

        exNodeList::iterator iter;
        for ( iter = FILES->begin(); iter != FILES->end(); iter++ ) {    				//Update the model for the view

                this->getQueue->append ( (*iter) );

                QList<QStandardItem*> items;
                items.append(  new QStandardItem( (*iter)->absPath ));
                items.append( new QStandardItem( this->get_curr_loc_path () + "/" +  (*iter)->relPath) );
                items.append( new QStandardItem( QString::number((*iter)->size) ) );
                items.append( new QStandardItem( ( (*iter)->type == 0 ? "File" : ( (*iter)->type == 1 ? "Folder" : "Symlink" ) ) ) );
                items.append( new QStandardItem( "Priority" ) );
                QStandardItem* progItem = new QStandardItem( "Progress" );
                items.append( progItem );
                this->model_transfers_queued->appendRow( items );
                this->widget.tabWidget_Transfers->setTabText( 0, "Queued (" + QString::number( this->model_transfers_queued->rowCount( ) ) + ")" );

                this->view_transfers_model->insert( (*iter), progItem);


        }

        int auto_process = 1;
        int concurrent_downloads = 1;
        if ( auto_process ){
                if ( this->dlQueueRunning < concurrent_downloads ) {
                        for ( int i=this->dlQueueRunning; i < concurrent_downloads; i++ ) {
                                this->dlQueueRunning++;
                                emit this->sg_get( this->getQueue->at ( this->dlQueueCurIndex ), this->get_curr_loc_path ( ) );
                                emit this->sg_get( this->getQueue->at ( this->dlQueueCurIndex ), this->get_curr_loc_path ( ) );
                                this->dlQueueCurIndex++;
                        }
                }
        }
}

void FMgr::sl_progressUpdate( exNode* node, int progress_perc ){
        //this->downloadsDelegate->UpdateProgress( row, progress_perc );
        QStandardItem* item = this->view_transfers_model->value( node );
        item->setText( QString::number(progress_perc) + "%");
        return;
}

void FMgr::sl_getDone( exNode* node, int progress_perc ){
        //this->downloadsDelegate->UpdateProgress( row, progress_perc );

        QStandardItem* item = this->view_transfers_model->value( node );
        item->setText( QString::number(progress_perc) + "%");

        this->completedQueue->append( node );							//Add the node to the completed model
        QList<QStandardItem*> itemRow = this->model_transfers_queued->takeRow( item->row( ) );  //Extract the row from the queued table view model
        this->model_transfers_completed->appendRow( itemRow ); 					//Add the item to the finished table view model

        this->widget.tabWidget_Transfers->setTabText( 0, "Queued (" + QString::number( this->model_transfers_queued->rowCount( ) ) + ")" );
        this->widget.tabWidget_Transfers->setTabText( 2, "Completed (" + QString::number( this->model_transfers_completed->rowCount( ) ) + ")" );

        this->dlQueueRunning--;									//Decrement by one process

        if ( this->dlQueueCurIndex >= ( this->getQueue->count( ) ) ) {
                this->dlQueueCurIndex = 0;
                this->dlQueueRunning = 0;
                return;
        }

        int concurrent_downloads = 1;
        for ( int i=this->dlQueueRunning; i < concurrent_downloads; i++ ) {
                this->dlQueueRunning++;
                emit this->sg_get( this->getQueue->at ( this->dlQueueCurIndex ), this->get_curr_loc_path ( ) );
                this->dlQueueCurIndex++;
         }

    qDebug() << QString::number(this->dlQueueCurIndex);
}


void FMgr::sl_triggered_remDownloadAction( ) {

        QItemSelectionModel* selections = widget.tableView_remote->selectionModel();		//Get all the current selections
        QModelIndexList selectedIndices = selections->selectedRows ();
        exNodeList* NODES = new exNodeList( );
        foreach ( QModelIndex index, selectedIndices ) {
                exNode* node = ::NodeToexNode( this->FILES->at( index.row( ) ) );
                NODES->append( node );
        }
        emit this->sg_get_ls ( NODES, this->get_curr_rem_path( ) );
}

void FMgr::eh_clicked_upload_loc_ctx_menu(){

    //Get all the current selections
    QItemSelectionModel* selections = widget.tableView_local->selectionModel();
	QModelIndexList selectedIndexes = selections->selectedRows(0);
	QStringList FILEINFOS;

     //Loop over every selected item
    foreach(QModelIndex index, selectedIndexes){
		
		//Get info on the selected item
		QString fileinfo = this->model_local->fileInfo(index).absoluteFilePath(); 
		FILEINFOS.append(fileinfo);

	}

    //QMetaObject::invokeMethod(this->transferMgr, "prepare_upload", Q_ARG(QStringList, FILEINFOS));

}

void FMgr::eh_loc_drives_cbox_changed(QString path){
        
    this->widget.tableView_local->setRootIndex(this->model_local->setRootPath(path));
    this->widget.txtBox_localpath->setText(path);
}

void FMgr::eh_loc_tableViewItemDoubleClicked(const QModelIndex& index){
    
    QString name = this->model_local->fileName(index);    

	//If the item is a folder 
    if(this->model_local->isDir(index)){
       //If the item is an up folder marker then move one folder level up
        if(name == ".."){            
            QString absPath = this->model_local->fileInfo(index).absoluteFilePath();
            QDir dir(absPath);
            dir.cdUp();            
            this->widget.tableView_local->setRootIndex(this->model_local->index(dir.path()));
            this->widget.txtBox_localpath->setText(dir.path());
        }else{
            
            QString absPath = this->model_local->fileInfo(index).absoluteFilePath();
            this->widget.tableView_local->setRootIndex(this->model_local->setRootPath(absPath)); 
            this->widget.txtBox_localpath->setText(absPath);
        }
        
        
    }
}

void FMgr::eh_rem_tableViewItemDoubleClicked( const QModelIndex& index ) {

        QModelIndex ind(index.model( )->index (index.row(), 0, index.parent( ) ) );			//Get the index of the selected row's first column
        QString node_name = ind.data( ).toString( );									//Get the value in the name column

        if(node_name != ".."){
                Node* node = this->FILES->at(index.row ( ) );							//Get the selected node from the data model based on row
                if ( node->type == 1) {                                 							//Is folder ?
                        this->add_to_rem_path( node->name );
                        this->widget.txtBox_remotepath->setText( this->get_curr_rem_path ( ) );
                        this->model_remote->removeRows(0, this->model_remote->rowCount()); 	//Clear all table rows
                        emit this->sg_ls ( this->get_curr_rem_path () );
                }else{														//Is file ?
                        exNodeList* nsList = new exNodeList();
                        exNode* nx = ::NodeToexNode ( node );
                        nsList->append (nx);
                        emit this->sg_get_ls ( nsList, this->get_curr_rem_path( ) );
                        return;
                }
        }else{
                this->rem_path_one_up( );
                QString newPath = this->get_curr_rem_path( );
                this->widget.txtBox_remotepath->setText( newPath );
                this->model_remote->removeRows(0, this->model_remote->rowCount()); 		//Clear all table rows
                emit this->sg_ls ( newPath );
        }

}

void FMgr::eh_fileReceived(){

    QMessageBox box;
    box.setText("DOne");
    box.exec();

}

void FMgr::eh_upload_job_prepared(Node *file){

	//Set the focus
	this->widget.tabWidget_Main->setCurrentIndex(1);
	this->widget.tabWidget_Transfers->setCurrentIndex(0);

    //this->widget.tableView_uploads->setModel(upload_job_model);
	
    //QMetaObject::invokeMethod(this->connection, "ssPUT", Q_ARG(ssFileSystemNodeList, JOB_QUEUE));

}

//void FMgr::traverseJobTree(ssNode* fileNode){

    /*QList<QStandardItem*> items;
	items.append(new QStandardItem(fileNode->name));
	items.append(new QStandardItem(fileNode->name));
	items.append(new QStandardItem(fileNode->size));
	items.append(new QStandardItem(""));
	items.append(new QStandardItem(""));
	this->model_uploads->appendRow(items);

	//Check for children nodes
	if(fileNode->children == NULL){
		return;
	}

   QList<ssNode* >::iterator i;
   for (i = fileNode->children->begin(); i != fileNode->children->end(); i++) {		
	   this->traverseJobTree((*i));
   }
	
   return;*/

//}

void FMgr::eh_logger_update(QByteArray data){
    this->widget.textEdit_log->append(QString(data).trimmed().toLatin1());
}

