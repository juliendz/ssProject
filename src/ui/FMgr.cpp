/*
 * File:   FMgr.cpp
 * Author: Aetion-LP
 * Created on May 26, 2012, 9:02 PM
 */

#include "FMgr.h"
#include <QMessageBox>

FMgr::FMgr(Session* sess) {
    
    widget.setupUi(this);
	this->widget.tabWidget_Main->setCurrentIndex(0);

    this->session = sess;
    this->rem_PATH.append(sess->initpath);
    this->widget.txtBox_remotepath->setText(this->get_curr_rem_path());

    setupTableView();

    //tableview item dclick signal/slot
    connect(this->widget.tableView_remote, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(eh_rem_tableViewItemDoubleClicked(const QModelIndex&)));
    connect(this->widget.tableView_local, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(eh_loc_tableViewItemDoubleClicked(const QModelIndex&)));
    connect(this->widget.comboBox_drives, SIGNAL(currentIndexChanged(QString)), this, SLOT(eh_loc_drives_cbox_changed(QString)));
    
    connect(this->widget.tableView_local, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showLocCtxMenu(const QPoint &)));
    connect(this->widget.tableView_remote, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showRemCtxMenu(const QPoint &)));

	this->create_worker_thread();
 
}


FMgr::~FMgr() {
    delete this->session;
    delete this->sshsession;
    delete this->worker;
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
    model_remote->setHorizontalHeaderItem(3, new QStandardItem(QString("Date")));
    model_remote->setHorizontalHeaderItem(4, new QStandardItem(QString("Time")));
    model_remote->setHorizontalHeaderItem(5, new QStandardItem(QString("Permissions")));
    this->widget.tableView_remote->setModel(model_remote);

	//Set the model for uploads view
    model_uploads = new QStandardItemModel(0,5,this);
    model_uploads->setHorizontalHeaderItem(0, new QStandardItem(QString("Local File")));
    model_uploads->setHorizontalHeaderItem(1, new QStandardItem(QString("Remote File")));
    model_uploads->setHorizontalHeaderItem(2, new QStandardItem(QString("Size")));
    model_uploads->setHorizontalHeaderItem(3, new QStandardItem(QString("Priority")));
    model_uploads->setHorizontalHeaderItem(4, new QStandardItem(QString("Progress")));
	this->widget.tableView_uploads->setModel(model_uploads);
	this->widget.tableView_uploads->setItemDelegate(new SessionsTableViewDelegate(this));


	//Set the model for downloads view
    model_downloads = new QStandardItemModel(0,5,this);
    model_downloads->setHorizontalHeaderItem(0, new QStandardItem(QString("Remote File")));
    model_downloads->setHorizontalHeaderItem(1, new QStandardItem(QString("Local File")));
    model_downloads->setHorizontalHeaderItem(2, new QStandardItem(QString("Size")));
    model_downloads->setHorizontalHeaderItem(3, new QStandardItem(QString("Priority")));
    model_downloads->setHorizontalHeaderItem(4, new QStandardItem(QString("Progress")));
	this->widget.tableView_downloads->setModel(model_downloads);
   
}

void FMgr::create_worker_thread(){

    //Create the worker thread object (QThread)
    //this->worker = new ssWorker();

    //Create the ssh session object
    this->sshsession = new SSHSession(this->session);

    //Connections
    connect(this->sshsession, SIGNAL(receivedFileListing(QList<Node*>*)), this, SLOT(eh_fileListReceived(QList<Node*>*)));
    connect(this->sshsession, SIGNAL(receivedFileListing_ex(Node_ex_List*)), this, SLOT(eh_fileListReceived_ex(Node_ex_List*)));

    //Move the transfer manager object onto the new worker thread
    //this->sshsession->moveToThread(this->worker);

    //Start the worker thread event loop
    //this->worker->start();
    
    this->call_connect();

}

void FMgr::destroy_worker_thread(){

}

void FMgr::call_connect(){
    //QMetaObject::invokeMethod(this->sshsession, "sftp_ls", Q_ARG(QString, this->get_curr_rem_path()));
    this->sshsession->sftp_ls(this->get_curr_rem_path());
}


QString FMgr::add_to_rem_path(QString path){
    
    this->rem_PATH.append(path+"/");
    return this->get_curr_rem_path();
}

QString FMgr::rem_from_rem_path(){
    
    this->rem_PATH.removeLast();
    return this->get_curr_rem_path();
}

QString FMgr::get_curr_rem_path(){
    return this->rem_PATH.join("");
}

void FMgr::showLocCtxMenu(const QPoint& pos){
       
        QMenu menu;
        
        QAction* act = menu.addAction("Upload");
        act->connect(act, SIGNAL(triggered()), this, SLOT(eh_clicked_upload_loc_ctx_menu()));        
        menu.addSeparator();
        menu.addAction("Open");
        menu.addAction("Delete");
        menu.addAction("Rename");
        menu.addSeparator();
        menu.addAction("Create directory");
        menu.addAction("Refresh");
        menu.exec(widget.tableView_local->mapToGlobal(pos));

}

void FMgr::showRemCtxMenu(const QPoint& pos){
    
    //Get the clicked column
    QModelIndex index = widget.tableView_remote->indexAt(pos); 
    QMap<int, QVariant> data = this->model_remote->itemData(index);    
    QString name = data[0].toString();
    
    //Get the sibling column from the row
    data = this->model_remote->itemData(index.sibling(index.row(), 5));
    QString perms = data[0].toString();
    
    //Is it a folder
    if(perms.startsWith("d")){
        
        QString foldername = name;
        
        QMenu menu;
        menu.addAction("Download directory");
        menu.addSeparator();
        menu.addAction("Enter");
        menu.addAction("Delete");
        menu.addAction("Rename");
        menu.addAction("Permissions");
        menu.addSeparator();
        menu.addAction("Create directory");
        menu.addAction("Refresh");
        menu.exec(widget.tableView_remote->mapToGlobal(pos));
        
    }else{
      
        QString filename = name;
        
        QMenu menu;
        menu.addAction("Download file");
        menu.addSeparator();
        menu.addAction("Open");
        menu.addAction("Delete");
        menu.addAction("Rename");
        menu.addAction("Permission");
        menu.addSeparator();
        menu.addAction("Create directory");
        menu.addAction("Refresh");
        menu.exec(widget.tableView_remote->mapToGlobal(pos));
    }

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

void FMgr::eh_fileListReceived(QList<Node *> *FILES){

    //Update the model for the view
    QList<Node*>::iterator iter;
    
    for(iter = FILES->begin(); iter != FILES->end(); iter++) {
        
        QList<QStandardItem*> items;
        items.append(new QStandardItem(QString((*iter)->name)));
        items.append(new QStandardItem(QString::number((*iter)->size)));
        items.append(new QStandardItem(""));
        items.append(new QStandardItem(""));
        items.append(new QStandardItem(""));
        items.append(new QStandardItem(QString((*iter)->perms)));
        this->model_remote->appendRow(items);
    }


}

void FMgr::eh_fileListReceived_ex(Node_ex_List *FILES){
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

void FMgr::eh_rem_tableViewItemDoubleClicked(const QModelIndex& index){

    //Get the clicked column
    QMap<int, QVariant> data = this->model_remote->itemData(index);
    QString foldername = data[0].toString();
    
    //Get the sibling column from the row
    data = this->model_remote->itemData(index.sibling(index.row(), 5));
    QString perms = data[0].toString();
    
    
    if(foldername != ".."){
        
        if(perms.startsWith("d")){                
            
            this->currUrl = this->add_to_rem_path(foldername);
            this->widget.txtBox_remotepath->setText(this->currUrl);                
        }else{
            
            QString filename = foldername;
            //QMetaObject::invokeMethod(this->connection, "ssGET", Q_ARG(QString, filename), Q_ARG(QString, this->currUrl));
            return;
        }
        
    }else{
        
        this->currUrl = this->rem_from_rem_path();
        this->widget.txtBox_remotepath->setText(this->currUrl);
    }
    
    //Remove all data rows
    this->model_remote->removeRows(0, this->model_remote->rowCount());
    
    
    //QMetaObject::invokeMethod(this->connection, "ssGET_FILE_LIST", Q_ARG(QString, this->currUrl));
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
