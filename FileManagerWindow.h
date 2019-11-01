#pragma once

#include <QtWidgets>

#include "Action.h"
#include "FileManager.h"
#include "Views/ProxyTreeView.h"
#include "PaneSwitcher.h"
#include "SearchPanel.h"

#include "ISettings.h"

namespace Ui {
class FileManagerWindow;
}

class Pane;
class QSortFilterProxyModel;
class Properties;

class FileManagerWindow : public QMainWindow, private ISettings {
    Q_OBJECT
public:
    explicit FileManagerWindow(QWidget* parent = nullptr);
    ~FileManagerWindow();

    FileManager* fileManager;

    ProxyTreeView* directoryTreeView;

    QMenu* contextSelectionMenu;        //Menu when selection exists
    QMenu* contextDirectoryMenu;        //Menu when no selection
    QMenu* contextEmptyMenu;            //Menu for directory tree

    QToolBar* topToolBar;                  //toolbar with list of available pane views

    SearchPanel* searchPanel;
    PaneSwitcher * paneSwitcher;

    TextEditorWindow* manualEditor;
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
    void loadFile(const QModelIndex&);

private slots:

    void slotTreeSelectionChanged(const QModelIndex&, const QModelIndex&);
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
    void closeEvent(QCloseEvent* event);
    void keyPressEvent(QKeyEvent* event);

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

    void saveState(QSettings*) const;
    QSettings* restoreState();

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

    Ui::FileManagerWindow *ui;

};
