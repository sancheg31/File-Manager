#pragma once

#include <QFileInfo>
#include <QMainWindow>
#include <QTableWidget>
#include <QSettings>

#include "Interfaces/ISettings.h"

class QAction;
class QLabel;
class FindDialog;
class Spreadsheet;


namespace Ui {
class SpreadsheetWindow;
}

class SpreadsheetWindow : public QMainWindow, private ISettings {
    Q_OBJECT

public:
    explicit SpreadsheetWindow(QWidget *parent = nullptr);
    ~SpreadsheetWindow();

public slots:
    void loadFile(const QFileInfo&);
protected:
    void closeEvent(QCloseEvent *);

private slots:

    void slotNewFile();
    void slotOpen();
    bool slotSave();
    bool slotSaveAs();

    void slotFind();
    void slotGoToCell();
    void slotSort();

    void slotAbout();
    void slotShow();

    void slotNewRow();
    void slotNewColumn();
    void slotDeleteRow();
    void slotDeleteColumn();

    void slotOpenRecentFile();
    void slotUpdateToolBar();
    void slotSpreadsheetModified();
    void slotLineEdited();

private:

    static const int MaxRecentFiles = 5;
    Ui::SpreadsheetWindow *ui;

    void createActions();
    void createOperationMenus();
    void createMenus();
    void createContextMenu();
    void createToolBars();
    void createStatusBar();

    QSettings* restoreState();
    void saveState(QSettings*) const;

    bool okToContinue();
    bool loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

    Spreadsheet* sheet;
    QTableWidget* table;
    FindDialog* findDialog;
    QLabel* locationLabel;
    QLineEdit* formulaEdit;
    QSettings* settings;
    QStringList recentFile;
    QString curFile;

    QAction *separatorAction;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *selectSubMenu;
    QMenu *toolsMenu;
    QMenu *optionsMenu;
    QMenu *helpMenu;

    QToolBar* fileToolBar;
    QToolBar* editToolBar;
    QToolBar* dataToolBar;

    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;

    QAction *recentFileActions[MaxRecentFiles];

    QAction *exitAction;

    QAction* newColumnAction;
    QAction* newRowAction;
    QAction* deleteRowAction;
    QAction* deleteColumnAction;

    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *deleteAction;

    QAction *selectAction;
    QAction *rowAction;
    QAction *columnAction;
    QAction *allAction;

    QAction *findAction;
    QAction *goToCellAction;

    QAction *recalculateAction;
    QAction *sortAction;

    QAction *showGridAction;
    QAction *autoRecalculateAction;

    QAction *aboutAction;

};

