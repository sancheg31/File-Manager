#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>
#include <QAbstractItemView>

#include "DefaultViewSettings.h"

class TreeView: public QTreeView, public DefaultViewSettings {
public:
    TreeView(QWidget * = nullptr);
};

#endif // TREEVIEW_H
