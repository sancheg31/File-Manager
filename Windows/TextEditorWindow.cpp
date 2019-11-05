
#include "TextEditorWindow.h"
#include "Action.h"

TextEditorWindow::TextEditorWindow(QWidget * wgt): QMainWindow(wgt), textEditor(new TextEditor),
    menuWindows(new QMenu("&Windows")), area(new QMdiArea)
{

    connect(textEditor, SIGNAL(fileAboutToBeClosed(IDocument*)), this, SLOT(slotFileAboutToBeClosed(IDocument*)));
    setWindowTitle("Text Editor");
    setWindowIcon(QIcon(":/Images/Notepad.png"));

    QToolBar* topToolBar = new QToolBar;

    menuBar()->addMenu(createFileMenu(topToolBar));
    menuBar()->addMenu(createEditMenu(topToolBar));
    menuBar()->addMenu(createWindowsMenu());
    menuBar()->addMenu(createFormatMenu());
    menuBar()->addMenu(createHelpMenu());
    addToolBar(Qt::TopToolBarArea, topToolBar);

    area = new QMdiArea;
    connect(area, SIGNAL(subWindowActivated(QMdiSubWindow*)), SLOT(slotSetActiveDocument(QMdiSubWindow*)));
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setCentralWidget(area);
    mapper = new QSignalMapper(this);
    connect(mapper, SIGNAL(mapped(QWidget*)), this, SLOT(slotSetActiveSubWindow(QWidget*)));
    settings = restoreState();
    statusBar()->showMessage("Ready", 3000);
}

QSettings* TextEditorWindow::restoreState() {
    QSettings* settings = textEditor->restoreState();
    restoreGeometry(settings->value("Geometry").toByteArray());
    for (auto & x: textEditor->getDocuments()) {
        area->addSubWindow(x);
        x->setWindowIcon(QIcon(":/Images/NewFile.png"));
        x->show();
    }
    return settings;
}

void TextEditorWindow::saveState(QSettings* settings) const {
    textEditor->saveState(settings);
    settings->setValue("Geometry", saveGeometry());
}

void TextEditorWindow::closeEvent(QCloseEvent * event) {
    saveState(settings);
    QMainWindow::closeEvent(event);
}

void TextEditorWindow::loadFile(const QFileInfo& fileInfo) {
    if (textEditor->loadFile(fileInfo)) {
        area->addSubWindow(textEditor->activeDocument());
        textEditor->activeDocument()->show();
    }
}


void TextEditorWindow::slotShow() {
    show();
    raise();
}

void TextEditorWindow::slotNewDoc() {
    textEditor->newDoc();
    area->addSubWindow(textEditor->activeDocument());
    textEditor->activeDocument()->show();
}

void TextEditorWindow::slotLoad() {
    QString str = QFileDialog::getOpenFileName(nullptr, tr("Open File"), tr("D:/"), tr("Files (*.txt)"));
    if (textEditor->load(str)) {
        area->addSubWindow(textEditor->activeDocument());
        textEditor->activeDocument()->show();
    }
}

void TextEditorWindow::slotSave() {
    if (textEditor->isNewFile())
        slotSaveAs();
    else
        textEditor->save();
}

void TextEditorWindow::slotSaveAs() {
    QString str = QFileDialog::getSaveFileName(nullptr, tr("Save File"), tr("D:/"), tr("Text Files (*.txt)"));
    textEditor->saveAs(str);
}

void TextEditorWindow::slotCut() {
    textEditor->cut();
}

void TextEditorWindow::slotCopy() {
    textEditor->copy();
}

void TextEditorWindow::slotPaste() {
    textEditor->paste();
}

void TextEditorWindow::slotUndo() {
    textEditor->undo();
}

void TextEditorWindow::slotDel() {
    textEditor->del();
}

void TextEditorWindow::slotSelectAll() {
    textEditor->selectAll();
}

void TextEditorWindow::slotFont() {
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, textEditor->activeDocument()->font(), nullptr, tr("Select Font"));
    if (ok)
        textEditor->setFont(font);
}

void TextEditorWindow::slotDateAndTime() {
    textEditor->setDateAndTime(QDateTime::currentDateTime());
}

void TextEditorWindow::slotWindows() {
    menuWindows->clear();
    QAction* action = menuWindows->addAction(QIcon(":/Images/Cascade.png"), "&Cascade", area, SLOT(cascadeSubWindows()));
    action->setStatusTip(tr("Cascade Files"));
    action->setEnabled(!area->subWindowList().isEmpty());

    action = menuWindows->addAction(QIcon(":/Images/Tile.png"), "&Tile", area, SLOT(tileSubWindows()));
    action->setStatusTip(tr("Tile Files"));
    action->setEnabled(!area->subWindowList().isEmpty());
    menuWindows->addSeparator();

    auto listDoc = area->subWindowList();
    for  (int i = 0; i < listDoc.size(); ++i) {
        action = menuWindows->addAction(listDoc.at(i)->windowTitle());
        action->setCheckable(true);
        action->setChecked(area->activeSubWindow() == listDoc.at(i));
        connect(action, SIGNAL(triggered()), mapper, SLOT(map()));
        mapper->setMapping(action, listDoc.at(i));
    }
}

void TextEditorWindow::slotAbout() {
    QMessageBox::about(this, "TextEditor", "Text Editor\nVersion 0.6.9");
}

void TextEditorWindow::slotSetActiveSubWindow(QWidget * wgt) {
    if (wgt) {
        qDebug() << "TextEditorWindow::slotSetActiveSubWindow(): casts are valid: " <<
                    qobject_cast<QMdiSubWindow*>(wgt) << " " << qobject_cast<IDocument*>(wgt);
        textEditor->setActiveDocument(textEditor->document(wgt->windowTitle()));
        area->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(wgt));
    } else {
        textEditor->setActiveDocument(nullptr);
    }
}

void TextEditorWindow::slotSetActiveDocument(QMdiSubWindow* win) {
    if (win) {
        textEditor->setActiveDocument(textEditor->document(win->widget()->windowTitle()));
    } else {
        textEditor->setActiveDocument(nullptr);
    }
}

void TextEditorWindow::slotFileAboutToBeClosed(IDocument* doc) {
    int res = QMessageBox::information(nullptr, "Text Editor", "Save file \"" + textEditor->activeDocument()->fileName() + "\" ?",
                                       QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
    if (res == QMessageBox::Yes)
        slotSave();
    else if (res == QMessageBox::No)
        doc->setState(IDocument::State::Closed);

}

//private methods

DocWindow* TextEditorWindow::activeDocWindow() const {
    auto temp = area->activeSubWindow();
    if (temp)
        return qobject_cast<DocWindow*>(area->activeSubWindow()->widget());
    return nullptr;
}

DocWindow* TextEditorWindow::docWindow(const QString& title) const {
    auto list = area->subWindowList();
    DocWindow* doc = nullptr;
    for (const auto & subWindow: list) {
        doc = qobject_cast<DocWindow*>(subWindow->widget());
        if (doc->windowTitle() == title)
            return doc;
    }
    return nullptr;
}

QMenu* TextEditorWindow::createFileMenu(QToolBar* topToolBar) const {

    Action* newAction = Action::create(QIcon(":/Images/NewFile.png"), "&Create", "Create New Document", QKeySequence::New);
    connect(newAction, SIGNAL(triggered()), this, SLOT(slotNewDoc()));

    Action* openAction = Action::create(QIcon(":/Images/Open.png"), "&Open...", "Open Existing Document", QKeySequence::Open);
    connect(openAction, SIGNAL(triggered()), this, SLOT(slotLoad()));

    Action* saveAction = Action::create(QIcon(":/Images/Save.png"), "&Save", "Save Document", QKeySequence::Save);
    saveAction->setDisabled(true);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(slotSave()));
    connect(&textEditor->getDocuments(), SIGNAL(containerIsEmpty(bool)), saveAction, SLOT(setDisabled(bool)));

    Action* saveAsAction = Action::create(QIcon(":/Images/SaveAs.png"), "&Save as...", "Save Document", QKeySequence::SaveAs);
    saveAsAction->setDisabled(true);
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(slotSaveAs()));
    connect(&textEditor->getDocuments(), SIGNAL(containerIsEmpty(bool)), saveAsAction, SLOT(setDisabled(bool)));

    Action* exitAction = Action::create(QIcon(":/Images/Exit.png"), tr("&Exit"), "Quit", QKeySequence::Quit);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    QMenu* fileMenu = new QMenu("&File");
    fileMenu->addActions(QList<QAction*>() << newAction << openAction << saveAction << saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);
    topToolBar->addActions(QList<QAction*>() << newAction << openAction << saveAction << saveAsAction);
    return fileMenu;
}

QMenu* TextEditorWindow::createEditMenu(QToolBar* topToolBar) const {

    Action* undoAction = Action::create(QIcon(":/Images/Undo.png"), "&Undo", "Undo Last Action", QKeySequence::Undo);
    undoAction->setDisabled(true);
    connect(undoAction, SIGNAL(triggered()), this, SLOT(slotUndo()));
    connect(&textEditor->getDocuments(), SIGNAL(containerIsEmpty(bool)), undoAction, SLOT(setDisabled(bool)));

    Action* cutAction = Action::create(QIcon(":/Images/Cut.png"), "&Cut", "Cut Text", QKeySequence::Cut);
    cutAction->setDisabled(true);
    connect(cutAction, SIGNAL(triggered()), this, SLOT(slotCut()));
    connect(&textEditor->getDocuments(), SIGNAL(containerIsEmpty(bool)), cutAction, SLOT(setDisabled(bool)));

    Action* copyAction = Action::create(QIcon(":/Images/Copy.png"), "&Copy", "Copy Text", QKeySequence::Copy);
    copyAction->setDisabled(true);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(slotCopy()));
    connect(&textEditor->getDocuments(), SIGNAL(containerIsEmpty(bool)), copyAction, SLOT(setDisabled(bool)));

    Action* pasteAction = Action::create(QIcon(":/Images/Paste.png"), "&Paste", "Paste Text", QKeySequence::Paste);
    pasteAction->setEnabled(false);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(slotPaste()));
    connect(copyAction, SIGNAL(triggered(bool)), pasteAction, SLOT(setDisabled(bool)));
    connect(cutAction, SIGNAL(triggered(bool)), pasteAction, SLOT(setDisabled(bool)));
    connect(&textEditor->getDocuments(), SIGNAL(containerIsEmpty(bool)), pasteAction, SLOT(setDisabled(bool)));

    Action* deleteAction = Action::create(QIcon(":/Images/Delete.png"), "&Delete", "Delete Text", QKeySequence::Delete);
    deleteAction->setDisabled(true);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(slotDel()));
    connect(&textEditor->getDocuments(), SIGNAL(containerIsEmpty(bool)), deleteAction, SLOT(setDisabled(bool)));

    Action* selectAllAction = Action::create(QIcon(":/Images/SelectAll.png"), "&Select All", "Select All Text", QKeySequence::SelectAll);
    selectAllAction->setDisabled(true);
    connect(selectAllAction, SIGNAL(triggered()), this, SLOT(slotSelectAll()));
    connect(&textEditor->getDocuments(), SIGNAL(containerIsEmpty(bool)), selectAllAction, SLOT(setDisabled(bool)));

    Action* dateTimeAction = Action::create(QIcon(":/Images/DateAndTime.png"), "&Date and Time", "Insert Date and Time", QKeySequence(Qt::Key_F5));
    dateTimeAction->setDisabled(true);
    connect(dateTimeAction, SIGNAL(triggered()), this, SLOT(slotDateAndTime()));
    connect(&textEditor->getDocuments(), SIGNAL(containerIsEmpty(bool)), dateTimeAction, SLOT(setDisabled(bool)));

    QMenu* editMenu = new QMenu("&Edit");
    editMenu->addActions(QList<QAction*>() << undoAction << cutAction << copyAction << pasteAction << deleteAction);
    editMenu->addSeparator();
    editMenu->addActions(QList<QAction*>() << selectAllAction << dateTimeAction);
    topToolBar->addActions(editMenu->actions());

    return editMenu;
}

QMenu* TextEditorWindow::createWindowsMenu() {
    menuWindows = new QMenu("&Windows");
    connect(menuWindows, SIGNAL(aboutToShow()), this, SLOT(slotWindows()));
    return menuWindows;
}

QMenu* TextEditorWindow::createFormatMenu() const {
    Action* fontAction = Action::create(QIcon(":/Images/Font.png"), "&Font...", "Set Font");
    connect(fontAction, SIGNAL(triggered()), this, SLOT(slotFont()));
    connect(&textEditor->getDocuments(), SIGNAL(containerIsEmpty(bool)), fontAction, SLOT(setDisabled(bool)));

    QMenu * formatMenu = new QMenu("&View");
    formatMenu->addActions(QList<QAction*>() << fontAction);
    return formatMenu;
}

QMenu* TextEditorWindow::createHelpMenu() const {
    Action* aboutAction = Action::create(QIcon(":/Images/About.png"), tr("&About"), "About", QKeySequence(Qt::Key_F1));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(slotAbout()));

    QMenu* helpMenu = new QMenu("&Help");
    helpMenu->addAction(aboutAction);
    return helpMenu;
}
