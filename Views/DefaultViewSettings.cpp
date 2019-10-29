#include "DefaultViewSettings.h"

QAbstractItemView* DefaultViewSettings::setDefaultViewSettings(QAbstractItemView * view) const {
    view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    view->setFrameStyle(QFrame::NoFrame | QFrame::Plain);
    view->setDragDropMode(QAbstractItemView::DragDrop);
    view->setDropIndicatorShown(true);
    view->setAcceptDrops(true);
    view->setDefaultDropAction(Qt::MoveAction);
    view->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked);
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    return view;
}

DefaultViewSettings::~DefaultViewSettings() {}
