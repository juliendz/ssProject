#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QDateTime>
#include <QList>
#include <QStandardItem>

struct Node{

    QString absPath;
    QString relPath;
    QString name;
    unsigned long size;
    int type;
    QString atime;
    QString mtime;
    QString perms;
    unsigned long uid;
    unsigned long gid;
    QList<Node* > *children;
    //QStandardItem* ui_model;

    ~Node(){
        delete children;
    }
};

struct exNode{
    QString name;
    QString absPath;
    QString relPath;
    short type;
    long size;
};


class exNodeList : public QList<exNode*>{

public:
    inline exNodeList(){}
    exNodeList(const exNodeList& other) {  }

};

inline exNode* NodeToexNode(Node* node ){
        exNode* n = new exNode();
        n->absPath = node->absPath;
        n->name = node->name;
        n->type = node->type;
        n->size = node->size;
        n->relPath = node->relPath;
        return n;
}

inline QString GetRelativePathWithoutName(QString relativePath, QString name){
        int startPos = 0;
        int endPos =  (relativePath.count( ) -  name.count ( ) ) - 1;
        QStringRef relPathWithoutName(&relativePath, startPos, endPos );
        return relPathWithoutName.toString( );
}

enum SSH_SESSION_STATE{
    SESSION_OPENING,
    SESSION_PERFORM_CONNECT,
    SESSION_SELECT_SOCKET,
    SESSION_PERFORM_INIT,
    SESSION_PERFORM_HANDSHAKE,
    SESSION_PERFORM_AUTH,
    SESSION_OPEN,
    SESSION_OPEN_ERROR,
    SESSION_CLOSED
};

enum SSH_CHANNEL_STATE{
    CHANNEL_OPENING,
    CHANNEL_OPEN,
    CHANNEL_OPEN_ERROR,
    CHANNEL_CLOSED,
    CHANNEL_OPERATION_INPROGRESS,
    CHANNEL_OPERATION_DONE,
    CHANNEL_OPERATION_ERROR,
    CHANNEL_IDLE,
    CHANNEL_INTERNAL_OPERATION_DONE
};

enum SSH_LS_OPERATION{
    LS,
        GET_LS,
        PUT_LS
};





#endif // COMMON_H
