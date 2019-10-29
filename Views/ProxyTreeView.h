#pragma once

#include <QTreeView>
#include <QSortFilterProxyModel>

class ProxyTreeView: public QTreeView {
public:
    ProxyTreeView(QWidget* = nullptr);
};
