#pragma once

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
    virtual ~SpreadsheetWindow() override;

protected:
    virtual void closeEvent(QCloseEvent *) override;

private slots:

    void slotNewFile();
    void slotOpen();
    bool slotSave();
    bool slotSaveAs();

    void slotFind();
    void slotGoToCell();
    void slotSort();

    void slotShowGrid();

    void slotAbout();
    void slotAboutQt();
    void slotShow();

    void slotOpenRecentFile();
    void slotUpdateStatusBar();
    void slotSpreadsheetModified();

private:

    static const int MaxRecentFiles = 5;
    Ui::SpreadsheetWindow *ui;

    void createActions();
    void createMenus();
    void createContextMenu();
    void createToolBars();
    void createStatusBar();

    virtual QSettings* restoreState() override;
    virtual void saveState(QSettings*) const override;

    bool okToContinue();
    bool loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

    Spreadsheet * sheet;
    QTableWidget * table;
    FindDialog *findDialog;
    QLabel *locationLabel;
    QLabel *formulaLabel;
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

    QToolBar *fileToolBar;
    QToolBar *editToolBar;

    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;

    QAction *recentFileActions[MaxRecentFiles];

    QAction *exitAction;

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

