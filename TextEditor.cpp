
#include <QtWidgets>

#include "Action.h"
#include "TextEditor.h"

TextEditor::TextEditor(QWidget * wgt): QMainWindow(wgt), bottomToolBar(new QToolBar), menuWindows(new QMenu("&Windows")),
                      settings(new QSettings("Kushnirenko, K-26", "MDI Text Editor")), searchPanel(new EditorSearchPanel()),
                      searchController(new FindReplaceController())
{
    setWindowTitle("MDI Text Editor");
    setWindowIcon(QIcon(":/Images/Notepad.ico"));

    connect(searchPanel->searchForLine, SIGNAL(textChanged(const QString&)), searchController, SLOT(changeFindLine(const QString&)));
    connect(searchPanel->replaceLine, SIGNAL(textChanged(const QString&)), searchController, SLOT(changeReplaceLine(const QString&)));
    connect(searchPanel->isCaseSensitive, SIGNAL(toggled(bool)), searchController, SLOT(changeSensivity(bool)));

    QToolBar* topToolBar = new QToolBar;
    bottomToolBar->addWidget(searchPanel);
    bottomToolBar->setVisible(true);

    menuBar()->addMenu(createFileMenu(topToolBar));
    menuBar()->addMenu(createEditMenu(topToolBar));
    menuBar()->addMenu(createWindowsMenu());
    menuBar()->addMenu(createFormatMenu());
    menuBar()->addMenu(createHelpMenu());
    addToolBar(Qt::TopToolBarArea, topToolBar);
    addToolBar(Qt::BottomToolBarArea, bottomToolBar);

    area = new QMdiArea;
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setCentralWidget(area);
    mapper = new QSignalMapper(this);
    connect(mapper, SIGNAL(mapped(QWidget*)), this, SLOT(slotSetActiveSubWindow(QWidget*)));
    restoreState();
    statusBar()->showMessage("Ready", 3000);

}

TextEditor::TextEditor(const QStringList& list, QWidget * wgt): TextEditor(wgt) {
    loadFiles(list);
}

TextEditor::TextEditor(const QList<QFileInfo>& list, QWidget * wgt): TextEditor(wgt) {
    loadFiles(list);
}

void TextEditor::loadFile(const QFileInfo& fileInfo) {
    if (!fileInfo.isFile() || activeFiles.contains(fileInfo.absoluteFilePath()))
        return;
    loadFiles(QStringList() << fileInfo.absoluteFilePath());
}

void TextEditor::slotShow() {
    this->show();
}

DocWindow* TextEditor::createNewDocument() {

    int counter = 1;
    QString title = QString("new 1");
    while (activeFiles.contains(title)) {
        title = QString("new %1").arg(++counter);
    }

    DocWindow* doc = new DocWindow;

    area->addSubWindow(doc);
    doc->setAttribute(Qt::WA_DeleteOnClose);
    doc->setWindowTitle(title);
    connect(doc, SIGNAL(fileNameChanged(const QString&, const QString&)), &activeFiles, SLOT(slotReplace(const QString&, const QString&)));
    connect(doc, SIGNAL(close(const QString&)), &activeFiles, SLOT(slotRemove(const QString&)));
    return doc;
}

void TextEditor::restoreState() {
    restoreGeometry(settings->value("Geometry").toByteArray());
    bottomToolBar->setVisible(settings->value("ShowBottomToolBar", QVariant(true)).toBool());
    int listCount(settings->value("FileNumber").toInt());

    for (int i = 0; i < listCount; ++i) {
        QString fileName(settings->value(QString("ActiveFile%1").arg(i)).toString());
        if (fileName.endsWith(".txt") && QFileInfo(fileName).isFile()) {
            loadFile(QFileInfo(fileName));
            activeFiles.insert(fileName);
        }
        else if (fileName.contains("new ") && !fileName.endsWith(".txt")) {
            QString plainText(settings->value(QString("ActiveFileInformation%1").arg(i)).toString());
            DocWindow* doc = createNewDocument();
            activeFiles.insert(doc->windowTitle());
            doc->setPlainText(plainText);
            doc->show();
        }
    }
}

void TextEditor::saveState() {
    auto list = activeFiles.toList();
    settings->setValue("Geometry", saveGeometry());
    settings->setValue("ShowBottomToolBar", bottomToolBar->isVisible());
    settings->setValue("FileNumber", list.count());

    int ind = 0;
    for (QList<QString>::iterator i = list.begin(); i != list.end(); ++i, ++ind) {
        settings->setValue(QString("ActiveFile%1").arg(ind), *i);
        if (i->contains("new ") && !i->endsWith(".txt")) {
            DocWindow* doc = docWindow(*i);
            if (doc != nullptr)
                settings->setValue(QString("ActiveFileInformation%1").arg(ind), doc->document()->toRawText());
        }
    }
}

void TextEditor::closeEvent(QCloseEvent * event) {
    saveState();
    QMainWindow::closeEvent(event);
}

void TextEditor::slotNewDoc() {
    DocWindow* doc = createNewDocument();
    activeFiles.insert(doc->windowTitle());
    doc->show();
}

void TextEditor::slotLoad() {
    QString str = QFileDialog::getOpenFileName(nullptr, tr("Open File"), tr("D:/"), tr("Files (*.txt)"));
    if (activeFiles.contains(str) || str.isEmpty())
        return;
    DocWindow* doc = createNewDocument();
    doc->slotLoad(str);
    activeFiles.insert(str);
    doc->show();
}

void TextEditor::slotSave() {
    DocWindow* doc = activeDocWindow();
    if (doc) {
        doc->slotSave();
    }
}

void TextEditor::slotSaveAs() {
    DocWindow* doc = activeDocWindow();
    if (doc) {
        doc->slotSaveAs();
    }
}

void TextEditor::slotCut() {
    DocWindow* doc = activeDocWindow();
    qDebug() << doc;
    if (doc) {
        doc->cut();
    }
}

void TextEditor::slotCopy() {
    DocWindow* doc = activeDocWindow();
    if (doc) {
        doc->copy();
    }
}

void TextEditor::slotPaste() {
    DocWindow* doc = activeDocWindow();
    if (doc)
        doc->paste();
}

void TextEditor::slotUndo() {
    DocWindow* doc = activeDocWindow();
    if (doc)
        doc->undo();
}

void TextEditor::slotDel() {
    DocWindow* doc = activeDocWindow();
    if (doc)
        doc->textCursor().removeSelectedText();
}

void TextEditor::slotFind() {
    bottomToolBar->setVisible(true);
    if (activeDocWindow() != searchController->getEdit())
        searchController->changeEdit(activeDocWindow());
    if (!searchController->findNext())
        QMessageBox::information(nullptr, "Warning", "Search reached end of file", QMessageBox::Ok);
}

void TextEditor::slotFindNext() {
    if (activeDocWindow() != searchController->getEdit())
        searchController->changeEdit(activeDocWindow());
    if (!searchController->findNext())
        QMessageBox::information(nullptr, "Warning", "Search reached end of file", QMessageBox::Ok);
}

void TextEditor::slotFindPrevious() {
    if (activeDocWindow() != searchController->getEdit())
        searchController->changeEdit(activeDocWindow());
    if (!searchController->findPrevious())
        QMessageBox::information(nullptr, "Warning", "Search reached beginning of file", QMessageBox::Ok);
}

void TextEditor::slotReplace() {
    if (activeDocWindow() != searchController->getEdit())
        searchController->changeEdit(activeDocWindow());
    searchController->replace();
    searchController->findNext();

}

void TextEditor::slotReplaceAndContinue() {
    slotReplace();
}

void TextEditor::slotReplaceAll() {
    if (activeDocWindow() != searchController->getEdit())
        searchController->changeEdit(activeDocWindow());
    while (searchController->findPrevious());
    while (searchController->replace())
        searchController->findNext();

}

void TextEditor::slotSelectAll() {
    DocWindow* doc = activeDocWindow();
    if (doc)
        doc->selectAll();
}

void TextEditor::slotDateAndTime() {
    DocWindow* doc = activeDocWindow();
    if (doc)
        doc->insertPlainText(QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate));
}

void TextEditor::slotFont() {
    DocWindow* doc = activeDocWindow();
    if (doc) {
        bool ok = false;
        QFont font = QFontDialog::getFont(&ok, doc->font(), nullptr, tr("Select Font"));
        if (ok)
            doc->setFont(font);
    }
}

void TextEditor::slotWindows() {
    menuWindows->clear();
    QAction* action = menuWindows->addAction(QIcon(":/Images/Cascade.ico"), "&Cascade", area, SLOT(cascadeSubWindows()));
    action->setStatusTip(tr("Cascade Files"));
    action->setEnabled(!area->subWindowList().isEmpty());

    action = menuWindows->addAction(QIcon(":/Images/Tile.ico"), "&Tile", area, SLOT(tileSubWindows()));
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

void TextEditor::slotAbout() {
    QMessageBox::about(this, "TextEditor", "Text Editor\nVersion 0.6.8");
}

void TextEditor::slotSetActiveSubWindow(QWidget * wgt) {
    if (wgt) {
        area->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(wgt));
    }
}

//private methods

DocWindow* TextEditor::activeDocWindow() const {
    auto temp = area->activeSubWindow();
    if (temp != nullptr)
        return qobject_cast<DocWindow*>(area->activeSubWindow()->widget());
    return nullptr;
}

DocWindow* TextEditor::docWindow(const QString& title) const {
    auto list = area->subWindowList();
    DocWindow* doc = nullptr;
    for (const auto & subWindow: list) {
        doc = qobject_cast<DocWindow*>(subWindow->widget());
        if (doc->windowTitle() == title)
            return doc;
    }
    return nullptr;
}

void TextEditor::loadFiles(const QStringList& list) {
    DocWindow* doc = nullptr;
    for (const auto & x: list) {
        if (activeFiles.contains(x))
            continue;
        doc = createNewDocument();
        doc->slotLoad(x);
        doc->show();
        activeFiles.insert(x);
    }
}

void TextEditor::loadFiles(const QList<QFileInfo>& list) {
    DocWindow* doc = nullptr;
    for (const auto & x: list) {
        if (activeFiles.contains(x.absoluteFilePath()))
            continue;
        doc = createNewDocument();
        doc->slotLoad(x.fileName());
        doc->show();
        activeFiles.insert(x.absoluteFilePath());

    }
}

QMenu* TextEditor::createFileMenu(QToolBar* topToolBar) const {

    Action* newAction = Action::create(QIcon(":/Images/NewFile.ico"), "&Create", "Create New Document", QKeySequence::New);
    connect(newAction, SIGNAL(triggered()), this, SLOT(slotNewDoc()));

    Action* openAction = Action::create(QIcon(":/Images/OpenFile.ico"), "&Open...", "Open Existing Document", QKeySequence::Open);
    connect(openAction, SIGNAL(triggered()), this, SLOT(slotLoad()));

    Action* saveAction = Action::create(QIcon(":/Images/Save.ico"), "&Save", "Save Document", QKeySequence::Save);
    connect(saveAction, SIGNAL(triggered()), this, SLOT(slotSave()));

    Action* saveAsAction = Action::create(QIcon(":/Images/SaveAs.ico"), "&Save as...", "Save Document", QKeySequence::SaveAs);
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(slotSaveAs()));

    Action* exitAction = Action::create(QIcon::fromTheme("application-exit", QIcon(":/Images/Exit.png")),
                                              tr("&Exit"), "Quit", QKeySequence::Quit);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    QMenu* fileMenu = new QMenu("&File");
    fileMenu->addActions(QList<QAction*>() << newAction << openAction << saveAction << saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);
    topToolBar->addActions(QList<QAction*>() << newAction << openAction << saveAction << saveAsAction);
    return fileMenu;
}

QMenu* TextEditor::createEditMenu(QToolBar* topToolBar) const {

    Action* undoAction = Action::create(QIcon(":/Images/Undo.ico"), "&Undo", "Undo Last Action", QKeySequence::Undo);
    connect(undoAction, SIGNAL(triggered()), this, SLOT(slotUndo()));
    connect(&activeFiles, SIGNAL(setIsEmpty(bool)), undoAction, SLOT(setDisabled(bool)));

    Action* cutAction = Action::create(QIcon(":/Images/Cut.png"), "&Cut", "Cut Text", QKeySequence::Cut);
    connect(cutAction, SIGNAL(triggered()), this, SLOT(slotCut()));
    connect(&activeFiles, SIGNAL(setIsEmpty(bool)), cutAction, SLOT(setDisabled(bool)));

    Action* copyAction = Action::create(QIcon(":/Images/Copy.png"), "&Copy", "Copy Text", QKeySequence::Copy);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(slotCopy()));
    connect(&activeFiles, SIGNAL(setIsEmpty(bool)), copyAction, SLOT(setDisabled(bool)));

    Action* pasteAction = Action::create(QIcon(":/Images/Paste.png"), "&Paste", "Paste Text", QKeySequence::Paste);
    pasteAction->setEnabled(false);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(slotPaste()));
    connect(copyAction, SIGNAL(triggered(bool)), pasteAction, SLOT(setDisabled(bool)));
    connect(cutAction, SIGNAL(triggered(bool)), pasteAction, SLOT(setDisabled(bool)));
    connect(&activeFiles, SIGNAL(setIsEmpty(bool)), pasteAction, SLOT(setDisabled(bool)));

    Action* deleteAction = Action::create(QIcon(":/Images/Delete.ico"), "&Delete", "Delete Text", QKeySequence::Delete);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(slotDel()));
    connect(&activeFiles, SIGNAL(setIsEmpty(bool)), deleteAction, SLOT(setDisabled(bool)));

    Action* findAction = Action::create(QIcon(":/Images/Find.ico"), "&Find", "Find Text", QKeySequence::Find);
    connect(findAction, SIGNAL(triggered()), this, SLOT(slotFind()));
    connect(&activeFiles, SIGNAL(setIsEmpty(bool)), findAction, SLOT(setDisabled(bool)));

    Action* findNextAction = Action::create(QIcon(":/Images/FindNext.ico"), "&Find Next", "Find Next Expression", QKeySequence::FindNext);
    connect(findNextAction, SIGNAL(triggered()), this, SLOT(slotFindNext()));
    connect(searchPanel->proceed, SIGNAL(clicked()), this, SLOT(slotFindNext()));
    connect(&activeFiles, SIGNAL(setIsEmpty(bool)), findNextAction, SLOT(setDisabled(bool)));

    Action* findPreviousAction = Action::create(QIcon(":/Images/FindPrevious.ico"), "&Find Previous", "Find Previous Expression", QKeySequence::FindPrevious);
    connect(findPreviousAction, SIGNAL(triggered()), this, SLOT(slotFindPrevious()));
    connect(searchPanel->previous, SIGNAL(clicked()), this, SLOT(slotFindPrevious()));
    connect(&activeFiles, SIGNAL(setIsEmpty(bool)), findPreviousAction, SLOT(setDisabled(bool)));

    Action* replaceAction = Action::create(QIcon(":/Images/Replace.ico"), "&Replace...", "Replace Text", QKeySequence::Replace);
    connect(replaceAction, SIGNAL(triggered()), this, SLOT(slotReplace()));
    connect(searchPanel->replace, SIGNAL(clicked()), this, SLOT(slotReplace()));
    connect(searchPanel->replaceAll, SIGNAL(clicked()), this, SLOT(slotReplaceAll()));
    connect(searchPanel->replaceContinue, SIGNAL(clicked()), this, SLOT(slotReplaceAndContinue()));
    connect(&activeFiles, SIGNAL(setIsEmpty(bool)), replaceAction, SLOT(setDisabled(bool)));

    Action* selectAllAction = Action::create(QIcon(":/Images/SelectAll.ico"), "&Select All", "Select All Text", QKeySequence::SelectAll);
    connect(selectAllAction, SIGNAL(triggered()), this, SLOT(slotSelectAll()));
    connect(&activeFiles, SIGNAL(setIsEmpty(bool)), selectAllAction, SLOT(setDisabled(bool)));

    Action* dateTimeAction = Action::create(QIcon(":/Images/DateAndTime.ico"), "&Date and Time", "Insert Date and Time", QKeySequence(Qt::Key_F5));
    connect(dateTimeAction, SIGNAL(triggered()), this, SLOT(slotDateAndTime()));
    connect(&activeFiles, SIGNAL(setIsEmpty(bool)), dateTimeAction, SLOT(setDisabled(bool)));

    QMenu* editMenu = new QMenu("&Edit");
    editMenu->addActions(QList<QAction*>() << undoAction << cutAction << copyAction << pasteAction << deleteAction);
    editMenu->addSeparator();
    editMenu->addActions(QList<QAction*>() << findAction << findNextAction << findPreviousAction << replaceAction);
    editMenu->addSeparator();
    editMenu->addActions(QList<QAction*>() << selectAllAction << dateTimeAction);
    topToolBar->addActions(editMenu->actions());

    return editMenu;
}

QMenu* TextEditor::createWindowsMenu() {
    menuWindows = new QMenu("&Windows");
    connect(menuWindows, SIGNAL(aboutToShow()), this, SLOT(slotWindows()));
    return menuWindows;
}

QMenu* TextEditor::createFormatMenu() const {
    Action* fontAction = Action::create(QIcon(":/Images/Font.ico"), "&Font...", "Set Font");
    connect(fontAction, SIGNAL(triggered()), this, SLOT(slotFont()));
    connect(&activeFiles, SIGNAL(setIsEmpty(bool)), fontAction, SLOT(setDisabled(bool)));

    QMenu * formatMenu = new QMenu("&View");
    formatMenu->addActions(QList<QAction*>() << fontAction);
    return formatMenu;
}

QMenu* TextEditor::createHelpMenu() const {
    Action* aboutAction = Action::create(QIcon::fromTheme("application-about", QIcon(":/Images/About.ico")),
                                     tr("&About"), "About", QKeySequence(Qt::Key_F1));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(slotAbout()));

    QMenu* helpMenu = new QMenu("&Help");
    helpMenu->addAction(aboutAction);
    return helpMenu;
}


