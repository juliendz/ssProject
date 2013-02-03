/*
 * File:   SessionsTableViewDelegate.h
 * Author: Julien
 * Created on July 22, 2012, 1:45 AM
 */

#ifndef TRANSFERSTABLEVIEWDELEGATE_H
#define	TRANSFERSTABLEVIEWDELEGATE_H

#include <QStyledItemDelegate>
#include <QApplication>
#include <QStyleOptionProgressBar>
#include <QMap>

class FMgr;

class TransfersTableViewDelegate: public QStyledItemDelegate {
    
public:
    TransfersTableViewDelegate(FMgr* parent);
    virtual ~TransfersTableViewDelegate();
    
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void UpdateProgress( int row, int progress_perc );
    
private:
    FMgr* parent;
    QMap<int, int> ROW_MAP;

};

#endif	/* TRANSFERSTABLEVIEWDELEGATE_H */

