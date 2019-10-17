#pragma once

#include <QtWidgets>

#include "EditorSearchPanel.h"
#include "DocWindow.h"
#include "StringSet.h"
#include "findReplaceController.h"

class QMdiArea;
class SearchPanel;

class TextEditor: public QMainWindow {
Q_OBJECT

public:
    TextEditor(QWidget * wgt = nullptr);
    TextEditor(const QList<QFileInfo>&, QWidget * wgt = nullptr);
    TextEditor(const QStringList&, QWidget * wgt = nullptr);

protected:
    void closeEvent(QCloseEvent *) override;

public slots:
    void loadFile(const QFileInfo&);
    void slotShow();
private slots:

    void slotUndo();
    void slotCut();
    void slotCopy();
    void slotPaste();
    void slotDel();
    void slotFind();
    void slotFindNext();
    void slotFindPrevious();
    void slotReplace();
    void slotReplaceAndContinue();
    void slotReplaceAll();
    void slotSelectAll();
    void slotDateAndTime();
    void slotFont();

    void slotNewDoc();
    void slotLoad();
    void slotSave();
    void slotSaveAs();
    void slotWindows();
    void slotAbout();
    void slotSetActiveSubWindow(QWidget*);

private:

    DocWindow* activeDocWindow() const;
    DocWindow* docWindow(const QString&) const;
    QMenu* createFileMenu(QToolBar* = nullptr) const;
    QMenu* createEditMenu(QToolBar* = nullptr) const;
    QMenu* createWindowsMenu();
    QMenu* createFormatMenu() const;
    QMenu* createHelpMenu() const;

    void loadFiles(const QStringList&);
    void loadFiles(const QList<QFileInfo>&);

    void restoreState();
    void saveState();

    DocWindow* createNewDocument();

    QToolBar* bottomToolBar;

    QMenu* menuWindows;
    QMdiArea* area;
    QSettings* settings;
    QSignalMapper* mapper;

    EditorSearchPanel* searchPanel;
    StringSet activeFiles;

    FindReplaceController* searchController;

};

