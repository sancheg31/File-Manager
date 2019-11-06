

#include "FileManagerWindow.h"
#include "ui_FileManagerWindow.h"

#include "Action.h"
#include "Pane.h"
#include "Dialogs/Parameters.h"
#include "Dialogs/Properties.h"
#include "SearchPanel.h"
#include "ProxyModel.h"

FileManagerWindow::FileManagerWindow(QWidget *parent) : QMainWindow(parent), searchPanel(new SearchPanel()),
    manualEditor(new TextEditorWindow()), spreadsheet(new SpreadsheetWindow()), fileManager(new FileManager()),
    menuBar(new QMenuBar()), splitter(new QSplitter()), ui(new Ui::FileManagerWindow)
{

    paneSwitcher = new PaneSwitcher(createPane(), createPane());
    directoryTreeView = new ProxyTreeView(splitter);
    setMenuBar(menuBar);
    setWindowTitle(tr("File Manager, Kushnirenko"));
    createActionsAndMenus();

    connect(searchPanel, SIGNAL(fileLoaded(const QFileInfo&)), this, SLOT(loadFile(const QFileInfo&)));
    connect(searchPanel, SIGNAL(directoryLoaded(const QFileInfo&)), this, SLOT(loadDirectory(const QFileInfo&)));

    directoryTreeView->setModel(fileManager->proxyModel());
    directoryTreeView->setDefaultSettings();
    connect(directoryTreeView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotShowContextDirectoryMenu(const QPoint&)));

    connect(directoryTreeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
            this, SLOT(slotTreeSelectionChanged(const QModelIndex&, const QModelIndex&)));

    connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(slotFocusChanged(QWidget*, QWidget*)));

    splitter->addWidget(directoryTreeView);
    splitter->addWidget(leftPane());
    splitter->addWidget(rightPane());
    splitter->setHandleWidth(3);

    this->setCentralWidget(splitter);
    connect(QApplication::clipboard(), SIGNAL(changed(QClipboard::Mode)), this, SLOT(clipboardChanged()));
    settings = restoreState();
    connect(activePane()->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(slotActivePaneSelectionChanged(const QItemSelection&, const QItemSelection&)));

}

FileManagerWindow::~FileManagerWindow() {
    delete ui;
}

void FileManagerWindow::clipboardChanged() {
    pasteAction->setEnabled(QApplication::clipboard()->mimeData()->hasUrls());
}

void FileManagerWindow::loadDirectoryTree(const QModelIndex& index) {
    if (paneSwitcher->isActive( qobject_cast<Pane*>(sender())) )
        directoryTreeView->setCurrentIndex(fileManager->proxyModel()->mapFromSource(index));
}

void FileManagerWindow::loadDirectory(const QFileInfo& fileInfo) {
    activePane()->moveTo(fileInfo.absoluteFilePath());
    loadDirectoryTree(fileManager->index(fileInfo.absoluteFilePath()));
}

void FileManagerWindow::loadFile(const QFileInfo& fileInfo) {
    if (!fileInfo.isFile())
        return;
    manualEditor->loadFile(fileInfo);
    manualEditor->show();
    manualEditor->raise();
}

void FileManagerWindow::loadFile(const QModelIndex& index) {
    loadFile(fileManager->fileInfo(index));
}

void FileManagerWindow::loadSpreadsheet(const QFileInfo & fileInfo) {
    if (!fileInfo.isFile())
        return;
    spreadsheet->loadFile(fileInfo);
    spreadsheet->show();
    spreadsheet->raise();
}

void FileManagerWindow::slotFocusChanged(QWidget *, QWidget* now) {
    if (now == rightPane()->focusWidget() || now == rightPane()->pathLine()) {
        setActivePane(rightPane());
    }
    else if (now == leftPane()->focusWidget() || now == leftPane()->pathLine()) {
        setActivePane(leftPane());
    }
}

void FileManagerWindow::setActivePane(Pane* pane) {
    if (paneSwitcher->isActive(pane))
        return;
    disconnect(activePane()->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
               this, SLOT(slotActivePaneSelectionChanged(const QItemSelection&, const QItemSelection&)));
    paneSwitcher->setActivePane(pane);

    connect(pane->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(slotActivePaneSelectionChanged(const QItemSelection&, const QItemSelection&)));
    emit pane->selectionModel()->selectionChanged(pane->selectionModel()->selection(), QItemSelection());
    updateViewActions();
}

Pane* FileManagerWindow::activePane() const {
    return paneSwitcher->getPane(PaneSwitcher::ActivePane);
}

Pane* FileManagerWindow::leftPane() const {
    return paneSwitcher->getPane(PaneSwitcher::LeftPane);
}

Pane* FileManagerWindow::rightPane() const {
    return paneSwitcher->getPane(PaneSwitcher::RightPane);
}

void FileManagerWindow::slotTreeSelectionChanged(const QModelIndex& current, const QModelIndex&) {
    QFileInfo fileInfo = fileManager->fileInfo(fileManager->mapToSource(current));
    if(!fileInfo.exists())
        return;
    activePane()->moveTo(fileInfo.filePath());
}

void FileManagerWindow::slotActivePaneSelectionChanged(const QItemSelection& cur, const QItemSelection&) {
    deleteAction->setEnabled(!cur.isEmpty());
    cutAction->setEnabled(!cur.isEmpty());
    copyAction->setEnabled(!cur.isEmpty());
    QString str = QString("%1 items selected, row %2").arg(cur.count()).arg(
                activePane()->selectionModel()->currentIndex().row());
    statusBar()->clearMessage();
    statusBar()->showMessage(str, -1);
}

void FileManagerWindow::moveTo(const QString& path) {
    QModelIndex index = fileManager->mapFromSource(fileManager->index(path));
    directoryTreeView->selectionModel()->select(index, QItemSelectionModel::Select);
    activePane()->moveTo(path);
}

void FileManagerWindow::slotShowSearchPanel() {
    searchPanel->show();
    searchPanel->raise();
}

void FileManagerWindow::slotShowContextPaneMenu(const QModelIndexList& list, const QPoint& position) {
    if (list.count() != 0)
        contextSelectionMenu->exec(position);
    else
        contextEmptyMenu->exec(position);
}

void FileManagerWindow::slotShowContextDirectoryMenu(const QPoint& position) {
    contextDirectoryMenu->exec(directoryTreeView->mapToGlobal(position));
}

void FileManagerWindow::slotOpenFile() {
    QFileInfo fileInfo = fileManager->fileInfo(activePane()->selectionModel()->currentIndex());
    if (fileInfo.isFile()) {
        loadFile(fileInfo);
    }
}

void FileManagerWindow::slotOpenDir() {
    QFileInfo fileInfo = fileManager->fileInfo(activePane()->selectionModel()->currentIndex());
    if (!fileInfo.isDir())
        return;
    activePane()->slotDoubleClickedOnEntry(activePane()->selectionModel()->currentIndex());
}

void FileManagerWindow::slotCut() {
    if (!(focusWidget() == activePane()->focusWidget()))
        return;
    if (fileManager->cut(activePane()->selectionModel()->selectedIndexes())) {
        pasteAction->setData(Qt::DropAction::MoveAction);
        activePane()->selectionModel()->clear();
    }
}

void FileManagerWindow::slotCopy() {
    if (!(focusWidget() == activePane()->focusWidget())) {
        return;
    }
    if (fileManager->copy(activePane()->selectionModel()->selectedIndexes()))  {
        pasteAction->setData(Qt::DropAction::CopyAction);
    }
}

void FileManagerWindow::slotPaste() {
    Qt::DropAction cutOrCopy(Qt::DropAction(pasteAction->data().toInt()));
    pasteAction->setEnabled(cutOrCopy == Qt::DropAction::MoveAction ? false : true);
    if (focusWidget() == activePane()->focusWidget()) {
        fileManager->paste(activePane()->focusView()->rootIndex(), cutOrCopy);
    } else if (focusWidget() == directoryTreeView) {
        fileManager->paste(fileManager->mapToSource(directoryTreeView->currentIndex()), cutOrCopy);
    }
}

void FileManagerWindow::slotDel() {

    if (!(focusWidget() == activePane()->focusWidget()))
        return;

    QModelIndexList selectionList = activePane()->selectionModel()->selectedIndexes();
    if (selectionList.count() == 0)
        return;

    bool yesToAll = false;
    bool ok = false;
    bool confirm = true;

    for(int i = 0; i < selectionList.count(); ++i) {

        QFileInfo file(fileManager->fileInfo(selectionList.at(i)).filePath());
        if(file.isWritable()) {
            if(file.isSymLink())
                ok = QFile::remove(file.filePath());
            else {
                if(!yesToAll) {
                    if(confirm) {

                        int answer = QMessageBox::information(this, tr("Delete File"), "Are you sure you want to delete? <p><b>\"" + file.filePath() + "</b>?",
                                                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll);
                        if(answer == QMessageBox::YesToAll)
                            yesToAll = true;
                        if(answer == QMessageBox::No)
                            return;
                    }
                }
                ok = fileManager->del(selectionList.at(i));
            }
        }
        else if(file.isSymLink())
            ok = QFile::remove(file.filePath());
    }

    if(!ok)
        QMessageBox::information(this, tr("Not Deleted"), tr("Some Files Cannot be Deleted."));
}

void FileManagerWindow::slotNewFolder() {
    if (focusWidget() != activePane()->focusWidget())
        return;
    QAbstractItemView* currentView = activePane()->focusView();
    auto rootInd = currentView->rootIndex();
    if (!rootInd.isValid())
        return;
    QModelIndex newDir = fileManager->newFolder(rootInd);
    currentView->selectionModel()->setCurrentIndex(newDir, QItemSelectionModel::ClearAndSelect);
    currentView->edit(newDir);
}

void FileManagerWindow::slotNewTxt() {
    if (focusWidget() != activePane()->focusWidget())
        return;
    QAbstractItemView* currentView = activePane()->focusView();
    auto rootInd = currentView->rootIndex();
    if (!rootInd.isValid())
        return;
    QModelIndex fileIndex = fileManager->newTxt(rootInd);
    currentView->setCurrentIndex(fileIndex);
    currentView->edit(fileIndex);
}

void FileManagerWindow::slotToggleHidden() {
    auto mask = QDir::NoDot | QDir::AllEntries | QDir::System;
    if(hiddenAction->isChecked())
        mask |= QDir::Hidden;
     fileManager->setFilter(mask);
}

void FileManagerWindow::closeEvent(QCloseEvent *event) {
    saveState(settings);
    QMainWindow::closeEvent(event);
}

void FileManagerWindow::keyPressEvent(QKeyEvent *event) {
    if ((event->modifiers() & Qt::CTRL) &&
        !(event->modifiers() & Qt::SHIFT) && !(event->modifiers() & Qt::ALT)) {
        if (event->key() == Qt::Key_X)
            slotCut();
        else if (event->key() == Qt::Key_C)
            slotCopy();
        else if ((event->key() == Qt::Key_V) && QApplication::clipboard()->mimeData() != nullptr)
            slotPaste();
        else if ((event->key() == Qt::Key_D || event->key() == Qt::Key_Delete) &&
                    !activePane()->selectionModel()->selectedIndexes().isEmpty())
            slotDel();
        else if (event->key() == Qt::Key_N)
            slotNewFolder();
    }
    QMainWindow::keyPressEvent(event);
}

void FileManagerWindow::saveState(QSettings* settings) const {
    settings->setValue("Geometry", saveGeometry());
    settings->setValue("ShowStatusBar", statusBar()->isVisible());
    settings->setValue("ShowToolBar", topToolBar->isVisible());

    settings->setValue("MainSplitterSizes", splitter->saveState());
    settings->setValue("LeftPaneActive", paneSwitcher->isActive(leftPane()));

    settings->setValue("LeftPanePath", leftPane()->pathLine()->text());
    settings->setValue("LeftPaneFileListHeader", leftPane()->treeView->header()->saveState());
    settings->setValue("LeftPaneViewMode", leftPane()->focusViewIndex());

    settings->setValue("RightPanePath", rightPane()->pathLine()->text());
    settings->setValue("RightPaneFileListHeader", rightPane()->treeView->header()->saveState());
    settings->setValue("RightPaneViewMode", rightPane()->focusViewIndex());
    settings->setValue("ShowHidden", hiddenAction->isChecked());
}

QSettings* FileManagerWindow::restoreState() {
    QSettings * settings = new QSettings("Kushnirenko, K-26", "File Manager");
    restoreGeometry(settings->value("Geometry").toByteArray());
    topToolBar->setVisible(settings->value("ShowToolBar", QVariant(true)).toBool());
    statusBar()->setVisible(settings->value("ShowStatusBar", QVariant(false)).toBool());
    splitter->restoreState(settings->value("MainSplitterSizes").toByteArray());
    paneSwitcher->setActivePane(settings->value("LeftPaneActive", 1).toBool() ? leftPane() : rightPane());
    leftPane()->treeView->header()->restoreState(settings->value("LeftPaneFileListHeader").toByteArray());
    leftPane()->moveTo(settings->value("LeftPanePath", "").toString());
    leftPane()->setFocusViewIndex(settings->value("LeftPaneViewMode", 0).toInt());
    rightPane()->treeView->header()->restoreState(settings->value("RightPaneFileListHeader").toByteArray());
    rightPane()->moveTo(settings->value("RightPanePath", "").toString());
    rightPane()->setFocusViewIndex(settings->value("RightPaneViewMode", 0).toInt());
    hiddenAction->setChecked(settings->value("ShowHidden", false).toBool());
    slotToggleHidden();
    return settings;
}

void FileManagerWindow::updateViewActions() {

    switch (activePane()->focusViewIndex()) {
    case Pane::TreeViewMode:
        detailViewAction->setChecked(true);
        break;
    case Pane::ListViewMode:
        iconViewAction->setChecked(true);
        break;
    case Pane::TableViewMode:
        tileViewAction->setChecked(true);
        break;
    }
}

void FileManagerWindow::slotShowAboutCreatorBox() {
    QMessageBox::about(this, tr("About"),
                       tr("<h2>File Manager</h2>"
                          "<p><em>Version 0.7.1</em><br>"
                          "К-26<br>"
                          "made by Alex. Kushnirenko, 2019<br>"));
}

void FileManagerWindow::slotShowParameters() {
    Parameters * parameters = new Parameters(formParametersList(), this);
    connect(parameters, SIGNAL(parametersChanged(const QList<bool>&)), this, SLOT(slotParametersChanged(const QList<bool>&)));
    parameters->exec();
}

void FileManagerWindow::slotParametersChanged(const QList<bool>& list) {
    topToolBar->setVisible(list.at(0));
    statusBar()->setVisible(list.at(1));
}

void FileManagerWindow::slotShowProperties() {
    if (activePane()->selectionModel()->selectedIndexes().count()) {
        QModelIndex index = activePane()->selectionModel()->selectedIndexes().at(0);
        const QFileInfo& fileInfo = fileManager->fileInfo(index);
        Properties properties(fileInfo, this);
        properties.exec();
    }
}

//private methods

QList<QPair<QString, bool>> FileManagerWindow::formParametersList() const {
    QList<QPair<QString, bool>> list;
    list.push_back(QPair{tr("Show View Panel"), topToolBar->isVisible()});
    list.push_back(QPair{tr("Show Status Bar"), statusBar()->isVisible()});
    return list;
}

void FileManagerWindow::createActionsAndMenus() {

    createActions();
    createViewChangeActions();
    createViewActionGroup();

    createMenus();
    createTopToolBar();
    createContextMenus();

}

void FileManagerWindow::createActions() {

    deleteAction = Action::create(QIcon(":/Images/Delete.png"), tr("&Delete"),
                                  "Delete File", QKeySequence::Delete);
    deleteAction->setEnabled(false);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(slotDel()));

    cutAction = Action::create(QIcon(":/Images/Cut.png"), tr("&Cut"),
                            "Cut File", QKeySequence::Cut);
    cutAction->setEnabled(false);
    connect(cutAction, SIGNAL(triggered()), this, SLOT(slotCut()));

    copyAction = Action::create(QIcon(":/Images/Copy.png"), tr("&Copy"),
                             "Copy File", QKeySequence::Copy);
    copyAction->setEnabled(false);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(slotCopy()));

    pasteAction = Action::create(QIcon(":/Images/Paste.png"), tr("Paste"),
                                 "Paste File", QKeySequence::Paste);
    pasteAction->setEnabled(false);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(slotPaste()));

    newFolderAction = Action::create(QIcon(":/Images/NewFolder.png"), tr("&New Folder"),
                                     "Create New Folder", QKeySequence::New);
    connect(newFolderAction, SIGNAL(triggered()), this, SLOT(slotNewFolder()));

    newTxtAction = Action::create(QIcon(":/Images/NewFile.png"), tr("&New File"),
                                  "Create New File");
    connect(newTxtAction, SIGNAL(triggered()), this, SLOT(slotNewTxt()));

    openAction = Action::create(QIcon(":/Images/Open.png"), tr("&Open"), "Open Element", QKeySequence::Open);
    connect(openAction, SIGNAL(triggered()), this, SLOT(slotOpenFile()));
    connect(openAction, SIGNAL(triggered()), this, SLOT(slotOpenDir()));

    parametersAction = Action::create(QIcon(":/Images/Parameters.png"), tr("&Parameters"),
                                    "Set Parameters", QKeySequence::Preferences);
    connect(parametersAction, SIGNAL(triggered()), this, SLOT(slotShowParameters()));

    exitAction = Action::create(QIcon(":/Images/Exit.png"), tr("&Quit"),
                             "Quit", QKeySequence::Quit);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    hiddenAction = Action::create(QIcon(":/Images/Hidden.png"), tr("Hidden Files"), "Show Hidden Files");
    hiddenAction->setCheckable(true);
    connect(hiddenAction, SIGNAL(triggered()), this, SLOT(slotToggleHidden()));

    openEditorAction = Action::create(QIcon(":/Images/Notepad.png"), tr("&File Editor"),
                                      "Open File Editor");
    connect(openEditorAction, SIGNAL(triggered()), manualEditor, SLOT(slotShow()));

    openSpreadsheetAction = Action::create(QIcon(":/Images/Spreadsheet.png"), tr("&Spreadsheet"), "Open Spreadsheet");
    connect(openSpreadsheetAction, SIGNAL(triggered()), spreadsheet, SLOT(slotShow()));

    aboutCreatorAction = Action::create(QIcon(":/Images/About.png"), tr("&About"), "About");
    connect(aboutCreatorAction, SIGNAL(triggered()), this, SLOT(slotShowAboutCreatorBox()));

    propertiesAction = Action::create(QIcon(":/Images/About.png"), tr("&Properties"),
                                   "Properties", QKeySequence(Qt::CTRL + Qt::Key_R));
    connect(propertiesAction, SIGNAL(triggered()), this, SLOT(slotShowProperties()));
}

void FileManagerWindow::createMenus() {

    if (menuBar == nullptr)
        return;

    fileMenu = menuBar->addMenu(tr("&File"));
    fileMenu->addActions(QList<QAction*>() << openSpreadsheetAction << openEditorAction << newTxtAction << newFolderAction <<
                         deleteAction << parametersAction);

    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar->addMenu(tr("&Edit"));
    editMenu->addActions(QList<QAction*>() << cutAction << copyAction << pasteAction);

    viewMenu = menuBar->addMenu(tr("&View"));
    viewMenu->addActions(QList<QAction*>() << detailViewAction << iconViewAction << tileViewAction << hiddenAction);

    helpMenu = menuBar->addMenu(tr("&Enquiry"));
    helpMenu->addActions(QList<QAction*>() << aboutCreatorAction);

}

void FileManagerWindow::createViewActionGroup() {
    if (detailViewAction == nullptr || iconViewAction == nullptr || tileViewAction == nullptr)
        return;
    viewActionGroup = new QActionGroup(this);
    viewActionGroup->addAction(detailViewAction);
    viewActionGroup->addAction(iconViewAction);
    viewActionGroup->addAction(tileViewAction);
}

void FileManagerWindow::createTopToolBar() {
    QPushButton* openSearchPanel = new QPushButton("Search");
    openSearchPanel->setFlat(true);
    connect(openSearchPanel, SIGNAL(clicked()), this, SLOT(slotShowSearchPanel()));
    topToolBar = new QToolBar;
    topToolBar->addActions(QList<QAction*>() << detailViewAction << iconViewAction << tileViewAction);
    topToolBar->addSeparator();
    topToolBar->addWidget(openSearchPanel);
    addToolBar(Qt::TopToolBarArea, topToolBar);
}

Pane* FileManagerWindow::createPane() {
    Pane * p = new Pane(fileManager->sourceModel(), splitter);
    connect(p, SIGNAL(fileLoaded(const QFileInfo&)), this, SLOT(loadFile(const QFileInfo&)));
    connect(p, SIGNAL(directoryLoaded(const QModelIndex&)), this, SLOT(loadDirectoryTree(const QModelIndex&)));
    connect(p, SIGNAL(spreadsheetLoaded(const QFileInfo&)), this, SLOT(loadSpreadsheet(const QFileInfo&)));
    connect(p, SIGNAL(contextMenuChanged(const QModelIndexList&, const QPoint&)),
            this, SLOT(slotShowContextPaneMenu(const QModelIndexList&, const QPoint&)));
    return p;
}

void FileManagerWindow::createViewChangeActions() {

    detailViewAction = Action::create(QIcon(":/Images/List.png"), tr("Table"), "Table View");
    detailViewAction->setCheckable(true);
    connect(detailViewAction, SIGNAL(triggered()), paneSwitcher, SLOT(slotToggleToDetailView()));

    iconViewAction = Action::create(QIcon(":/Images/Table.png"), tr("List"), "List View");
    iconViewAction->setCheckable(true);
    connect(iconViewAction, SIGNAL(triggered()), paneSwitcher, SLOT(slotToggleToIconView()));

    tileViewAction = Action::create(QIcon(":/Images/Icons.png"), tr("Tile"), "Tile View");
    tileViewAction->setCheckable(true);
    connect(tileViewAction, SIGNAL(triggered()), paneSwitcher, SLOT(slotToggleToTileView()));
}

void FileManagerWindow::createContextMenus() {
    createContextSelectionMenu();
    createContextEmptyMenu();
    createContextDirectoryMenu();
}

void FileManagerWindow::createContextSelectionMenu() {
    contextSelectionMenu = new QMenu(this);
    contextSelectionMenu->addAction(openAction);
    contextSelectionMenu->addSeparator();
    contextSelectionMenu->addActions(QList<QAction*>() << detailViewAction << iconViewAction << tileViewAction);
    contextSelectionMenu->addSeparator();
    contextSelectionMenu->addAction(propertiesAction);
    contextSelectionMenu->addSeparator();
    contextSelectionMenu->addActions(QList<QAction*>() << cutAction << copyAction << pasteAction);
    contextSelectionMenu->addSeparator();
    Action *delete2Action = Action::create(QIcon(":/Images/Delete.png"), tr("&Delete"), "Delete File", QKeySequence::Delete);
    connect(delete2Action, SIGNAL(triggered()), this, SLOT(slotDel()));
    contextSelectionMenu->addAction(delete2Action);
}

void FileManagerWindow::createContextEmptyMenu() {
    contextEmptyMenu = new QMenu(this);
    contextEmptyMenu->addActions(QList<QAction*>() << newTxtAction << newFolderAction);
    contextEmptyMenu->addSeparator();
    contextEmptyMenu->addActions(QList<QAction*>() << detailViewAction << iconViewAction << tileViewAction);
    contextEmptyMenu->addSeparator();
    contextEmptyMenu->addAction(pasteAction);
}

void FileManagerWindow::createContextDirectoryMenu() {
    contextDirectoryMenu = new QMenu(this);
    contextDirectoryMenu->addActions(QList<QAction*>() << copyAction << pasteAction);
}

