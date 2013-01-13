/* 
 * File:   SessionsTableViewDelegate.h
 * Author: Julien
 * Created on July 22, 2012, 1:45 AM
 */

#ifndef SESSIONSTABLEVIEWDELEGATE_H
#define	SESSIONSTABLEVIEWDELEGATE_H

#include <QStyledItemDelegate>
#include <QApplication>
#include <QStyleOptionProgressBar>

class FMgr;

class SessionsTableViewDelegate: public QStyledItemDelegate {
    
public:
    SessionsTableViewDelegate(FMgr* parent);
    virtual ~SessionsTableViewDelegate();
    
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    
private:
	FMgr* parent;

};

#endif	/* SESSIONSTABLEVIEWDELEGATE_H */

