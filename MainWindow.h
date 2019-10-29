#pragma once

#include <QtWidgets>

#include "Action.h"
#include "Views/ProxyTreeView.h"
#include "TextEditor.h"
#include "PaneSwitcher.h"
#include "SearchPanel.h"
#include "SpreadsheetWindow.h"

#include "ISettings.h"

class Pane;
class QSortFilterProxyModel;
class Properties;
class PreviewPane;
class PathValidator;

class MainWindow : public QMainWindow, private ISettings {
    Q_OBJECT
public:

    MainWindow(QWidget* parent = nullptr);

    QFileSystemModel* fileSystemModel;
    QSortFilterProxyModel* fileSystemProxyModel;

    ProxyTreeView* directoryTreeView;

    QMenu* contextSelectionMenu;        //Menu when selection exists
    QMenu* contextDirectoryMenu;        //Menu when no selection
    QMenu* contextEmptyMenu;            //Menu for directory tree

    QToolBar* topToolBar;                  //toolbar with list of available pane views

    SearchPanel* searchPanel;
    PaneSwitcher * paneSwitcher;

    TextEditor* manualEditor;

    SpreadsheetWindow* spreadsheet;

    void setActivePane(Pane* pane);

    Pane* activePane() const;
    Pane* leftPane() const;
    Pane* rightPane() const;
    QItemSelectionModel* treeSelectionModel() const;

    void moveTo(const QString& path);
    void updateViewActions();

public slots:
    void clipboardChanged();
    void loadDirectoryTree(const QModelIndex&);
    void loadDirectory(const QFileInfo&);
    void loadFile(const QFileInfo&);

private slots:

    void slotTreeSelectionChanged(const QModelIndex& current, const QModelIndex& previous);
    void slotActivePaneSelectionChanged(const QItemSelection&, const QItemSelection&);

    void slotCut();
    void slotCopy();
    void slotPaste();
    void slotDel();
    void slotNewFolder();
    void slotNewTxt();
    void slotOpenFile();
    void slotOpenDir();

    void slotParametersChanged(const QList<bool>&);
    void slotFocusChanged(QWidget *, QWidget *now);

    void slotToggleHidden();
    void slotShowSearchPanel();
    void slotShowContextPaneMenu(const QModelIndexList& list, const QPoint&);
    void slotShowContextDirectoryMenu(const QPoint&);

    void slotShowAboutCreatorBox();
    void slotShowParameters();
    void slotShowProperties();

protected:
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:

    QList<QPair<QString, bool>> formParametersList() const;

    Pane* createPane();
    void createMenus();
    void createActions();
    void createActionsAndMenus();
    void createViewChangeActions();
    void createViewActionGroup();
    void createTopToolBar();

    void createContextMenus();
    void createContextSelectionMenu();
    void createContextEmptyMenu();
    void createContextDirectoryMenu();

    virtual void saveState(QSettings*) const override;
    virtual QSettings* restoreState() override;

    //for copy purposes only

    QModelIndexList selectionList;
    QString sourceCopyPath;
    void copyFolders(const QModelIndexList& list, const QString& dst);
    void copyPath(const QString& src, const QString& dst);

    //Menus

    QMenuBar* menuBar;
    QMenu* fileMenu;
    QMenu* editMenu;
    QMenu* viewMenu;
    QMenu* helpMenu;

    Action* detailViewAction;
    Action* iconViewAction;
    Action* tileViewAction;
    QActionGroup* viewActionGroup;

    Action* openEditorAction;
    Action* openSpreadsheetAction;
    Action* openAction;
    Action* newTxtAction;
    Action* newFolderAction;
    Action* cutAction;
    Action* copyAction;
    Action* pasteAction;
    Action* deleteAction;

    Action* propertiesAction;

    Action* exitAction;
    Action* parametersAction;
    Action* hiddenAction;

    Action* aboutCreatorAction;
    Action* aboutHotKeyAction;

    QSplitter* splitter;
    QSettings* settings;

};

