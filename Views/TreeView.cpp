#include "Views/TreeView.h"

TreeView::TreeView(QWidget* parent): QTreeView(parent) {
    setDefaultViewSettings(this);
    setRootIsDecorated(false);
    setItemsExpandable(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);
}
