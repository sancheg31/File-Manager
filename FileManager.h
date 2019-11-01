#pragma once

#include <QtWidgets>

#include "Action.h"
#include "Views/ProxyTreeView.h"
#include "PaneSwitcher.h"
#include "SearchPanel.h"
#include "SpreadsheetWindow.h"
#include "TextEditorWindow.h"

#include "ISettings.h"

class Pane;
class QSortFilterProxyModel;
class Properties;

class FileManager: public QObject {
    Q_OBJECT
public:
    FileManager(QObject* parent = nullptr);

    QFileSystemModel* fileSystemModel;
    QSortFilterProxyModel* fileSystemProxyModel;

    void updateViewActions();

    QFileInfo fileInfo(const QModelIndex& index) const;
    QModelIndex index(const QString&) const;
    QModelIndex mapToSource(const QModelIndex&) const;
    QModelIndex mapFromSource(const QModelIndex&) const;

    QSortFilterProxyModel* proxyModel() const;
    QFileSystemModel* sourceModel() const;

    void setFilter(QDir::Filters);

public:

    QModelIndex newFolder(const QModelIndex&);
    QModelIndex newTxt(const QModelIndex&);

    bool cut(const QModelIndexList&);
    bool copy(const QModelIndexList&);
    void paste(const QModelIndex&, Qt::DropAction);
    bool del(const QModelIndex&);
    void del(const QModelIndexList&);

private slots:
/*
    void slotNewFolder(const QModelIndex&);
    void slotNewTxt(const QModelIndex&);

    void slotCut(const QModelIndexList&);
    void slotCopy(const QModelIndexList&);
    void slotPaste(Qt::DropAction);
    void slotDel(const QModelIndex&);
    void slotDel(const QModelIndexList&);
*/
private:

    QModelIndexList selectionList;
    QString sourceCopyPath;
    void copyFolders(const QModelIndexList& list, const QString& dst);
    void copyPath(const QString& src, const QString& dst);

    QSettings* settings;

};

