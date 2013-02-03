/*
 * File:   SessionsTableViewDelegate.cpp
 * Author: Julien
 * Created on July 22, 2012, 1:45 AM
 */

#include "transferstableviewdelegate.h"

TransfersTableViewDelegate::TransfersTableViewDelegate(FMgr* parent) {
	this->parent = parent;
}


TransfersTableViewDelegate::~TransfersTableViewDelegate() {
}

void TransfersTableViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const{
        if( index.column() == 4 ) {
                QStyleOptionProgressBar progressBarStyleOption;
            progressBarStyleOption.rect = option.rect;
		progressBarStyleOption.minimum = 0;
		progressBarStyleOption.maximum = 100;
		progressBarStyleOption.textAlignment = Qt::AlignCenter;
            int progress = (this->ROW_MAP.contains ( index.row ( ) ) ? this->ROW_MAP.value ( index.row ( ) ) : 0 );
            progressBarStyleOption.progress = progress;
		progressBarStyleOption.text = QString( "%1%" ).arg( progress  );
		progressBarStyleOption.textVisible = true;
                qApp->style()->drawControl( QStyle::CE_ProgressBar, &progressBarStyleOption, painter );
    }
    else{
            QStyledItemDelegate::paint(painter, option, index);
    }
}

void TransfersTableViewDelegate::UpdateProgress ( int row, int progress_perc ) {
    this->ROW_MAP[row] = progress_perc;
    return;
}

