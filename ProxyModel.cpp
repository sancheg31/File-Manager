#include <QModelIndex>
#include <QFileSystemModel>

#include "ProxyModel.h"


bool FileSystemModelFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    QFileSystemModel* fileSystemModel = qobject_cast<QFileSystemModel*>(sourceModel());
    if (fileSystemModel->isDir(index) && fileSystemModel->fileName(index).compare("..") != 0)
        return true;
    else
        return false;
}

