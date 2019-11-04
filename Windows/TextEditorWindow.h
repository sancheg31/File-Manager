#pragma once

#include <QMainWindow>
#include <QMdiArea>
#include <QSignalMapper>

#include "Interfaces/ISettings.h"
#include "StringSet.h"
#include "TextEditor.h"

class TextEditorWindow: public QMainWindow, private ISettings {
    Q_OBJECT
public:
    TextEditorWindow(QWidget* = nullptr);

    void loadFile(const QFileInfo&);

protected:
    virtual void closeEvent(QCloseEvent *) override;

private slots:

    void slotShow();

    void slotNewDoc();
    void slotLoad();
    void slotSave();
    void slotSaveAs();

    void slotUndo();
    void slotCut();
    void slotCopy();
    void slotPaste();
    void slotDel();
    void slotSelectAll();
    void slotDateAndTime();
    void slotFont();

    void slotWindows();
    void slotAbout();
    void slotSetActiveSubWindow(QWidget*);
    void slotSetActiveDocument(QMdiSubWindow*);
    void slotFileAboutToBeClosed(IDocument*);

private:

    virtual QSettings* restoreState() override;
    virtual void saveState(QSettings*) const override;

    DocWindow* activeDocWindow() const;
    DocWindow* docWindow(const QString&) const;

    QMenu* createFileMenu(QToolBar* = nullptr) const;
    QMenu* createEditMenu(QToolBar* = nullptr) const;
    QMenu* createWindowsMenu();
    QMenu* createFormatMenu() const;
    QMenu* createHelpMenu() const;

    TextEditor* textEditor;

    QMenu* menuWindows;
    QMdiArea* area;
    QSettings* settings;
    QSignalMapper* mapper;

};
