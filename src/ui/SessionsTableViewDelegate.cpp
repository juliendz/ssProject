/* 
 * File:   SessionsTableViewDelegate.cpp
 * Author: Julien
 * Created on July 22, 2012, 1:45 AM
 */

#include "SessionsTableViewDelegate.h"

SessionsTableViewDelegate::SessionsTableViewDelegate(FMgr* parent) {
	this->parent = parent;
}


SessionsTableViewDelegate::~SessionsTableViewDelegate() {
}

void SessionsTableViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const{
    if(index.column() == 4){
        QStyleOptionProgressBar progressBarStyleOption;
		progressBarStyleOption.rect = option.rect;
	 
		const int progress = 10;
	 
		progressBarStyleOption.minimum = 0;
		progressBarStyleOption.maximum = 100;
		progressBarStyleOption.textAlignment = Qt::AlignCenter;
		progressBarStyleOption.progress = progress ;
		progressBarStyleOption.text = QString( "%1%" ).arg( progress  );
		progressBarStyleOption.textVisible = true;
	 
        qApp->style()->drawControl( QStyle::CE_ProgressBar, &progressBarStyleOption, painter );
    }
    else{
         QStyledItemDelegate::paint(painter, option, index);
    }
}

