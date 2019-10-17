#pragma once

#include <QtWidgets>

#include "ListView.h"
#include "TableView.h"
#include "TreeView.h"

class MainWindow;

class Pane : public QFrame {
    Q_OBJECT
public:
    explicit Pane(QWidget* parent = nullptr);
    explicit Pane(QFileSystemModel *, QWidget * parent = nullptr);

    TreeView* treeView;
    ListView* listView;
    TableView* tableView;

    enum ViewMode
    {
        TreeViewMode = 0,
        ListViewMode = 1,
        TableViewMode = 2
    };

    QLineEdit* pathLine() const;
    QItemSelectionModel* selectionModel() const;
    QAbstractItemView* focusView() const;
    QWidget* focusWidget() const;
    int focusViewIndex() const;

    void setFocusViewIndex(int);
    void setAlternatingRowColors(bool);

    void moveTo(const QString& path);
    void setViewTo(const ViewMode viewMode);

private:

    QFileSystemModel* fileModel;
    QLineEdit* pathLineEdit;

    QStackedWidget* stackedWidget;

public slots:
    void slotDoubleClickedOnEntry(const QFileInfo&);
    void slotDoubleClickedOnEntry(const QModelIndex&);

private slots:
    void slotShowContextMenu(const QPoint&);
    void slotPathLineEditChanged();

signals:
    void fileLoaded(const QFileInfo&);
    void directoryLoaded(const QModelIndex&);
    void contextMenuChanged(const QModelIndexList&, const QPoint&);
};

