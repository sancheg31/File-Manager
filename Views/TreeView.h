#pragma once

#include <QTreeView>
#include <QAbstractItemView>

#include "Views/DefaultViewSettings.h"

class TreeView: public QTreeView, public DefaultViewSettings {
public:
    TreeView(QWidget * = nullptr);
};

