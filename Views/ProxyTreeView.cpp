#include "ProxyTreeView.h"

ProxyTreeView::ProxyTreeView(QWidget *wgt): QTreeView(wgt) {

}

void ProxyTreeView::setDefaultSettings() {
    setHeaderHidden(true);
    setUniformRowHeights(true);
    hideColumn(1); hideColumn(2); hideColumn(3);
    setDragDropMode(QAbstractItemView::DropOnly);
    setDefaultDropAction(Qt::MoveAction);
    setDropIndicatorShown(true);
    setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked);
    setContextMenuPolicy(Qt::CustomContextMenu);
}
