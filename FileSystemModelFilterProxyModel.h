#pragma once

#include <QSortFilterProxyModel>

class QModelIndex;

class FileSystemModelFilterProxyModel : public QSortFilterProxyModel {
protected:
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
};


