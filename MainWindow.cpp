
#include "Action.h"
#include "MainWindow.h"
#include "Pane.h"
#include "Dialogs/Parameters.h"
#include "Dialogs/Properties.h"
#include "SearchPanel.h"
#include "FileSystemModelFilterProxyModel.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), fileSystemModel(new QFileSystemModel()),
    searchPanel(new SearchPanel()), manualEditor(new TextEditor()), spreadsheet(new SpreadsheetWindow()),
    menuBar(new QMenuBar()), splitter(new QSplitter())
{
    qDebug() << "MainWindow constructor starts";
    paneSwitcher = new PaneSwitcher(createPane(), createPane());
    directoryTreeView = new ProxyTreeView(splitter);
    setMenuBar(menuBar);
    setWindowTitle(tr("File Manager, Kushnirenko"));
    createActionsAndMenus();

    connect(searchPanel, SIGNAL(fileLoaded(const QFileInfo&)), this, SLOT(loadFile(const QFileInfo&)));
    connect(searchPanel, SIGNAL(directoryLoaded(const QFileInfo&)), this, SLOT(loadDirectory(const QFileInfo&)));
    fileSystemModel->setFilter(QDir::NoDot | QDir::AllEntries | QDir::System);
    fileSystemModel->setRootPath("");
    fileSystemModel->setReadOnly(false);

    fileSystemProxyModel = new FileSystemModelFilterProxyModel();
    fileSystemProxyModel->setSourceModel(fileSystemModel);
    fileSystemProxyModel->setSortCaseSensitivity(Qt::CaseSensitive);

    directoryTreeView->setModel(fileSystemProxyModel);
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
    qDebug() << "MainWindow constructor ends";
}

void MainWindow::clipboardChanged() {
    pasteAction->setEnabled(QApplication::clipboard()->mimeData()->hasUrls());
}

void MainWindow::loadDirectoryTree(const QModelIndex& index) {
    if (paneSwitcher->isActive( qobject_cast<Pane*>(sender())) )
        directoryTreeView->setCurrentIndex(fileSystemProxyModel->mapFromSource(index));
}

void MainWindow::loadDirectory(const QFileInfo& fileInfo) {
    activePane()->moveTo(fileInfo.absoluteFilePath());
    loadDirectoryTree(fileSystemModel->index(fileInfo.absoluteFilePath()));
}

void MainWindow::loadFile(const QFileInfo& fileInfo) {
    manualEditor->loadFile(fileInfo);
    manualEditor->show();
}

void MainWindow::slotFocusChanged(QWidget *, QWidget* now) {
    if (now == rightPane()->focusWidget() || now == rightPane()->pathLine()) {
        setActivePane(rightPane());
    }
    else if (now == leftPane()->focusWidget() || now == leftPane()->pathLine()) {
        setActivePane(leftPane());
    }
}

void MainWindow::setActivePane(Pane* pane) {

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

Pane* MainWindow::activePane() const {
    return paneSwitcher->getPane(PaneSwitcher::ActivePane);
}

Pane* MainWindow::leftPane() const {
    return paneSwitcher->getPane(PaneSwitcher::LeftPane);
}

Pane* MainWindow::rightPane() const {
    return paneSwitcher->getPane(PaneSwitcher::RightPane);
}

void MainWindow::slotTreeSelectionChanged(const QModelIndex& current, const QModelIndex&) {
    QFileInfo fileInfo = fileSystemModel->fileInfo(fileSystemProxyModel->mapToSource(current));
    if(!fileInfo.exists())
        return;
    activePane()->moveTo(fileInfo.filePath());
}

void MainWindow::slotActivePaneSelectionChanged(const QItemSelection& cur, const QItemSelection&) {
    deleteAction->setEnabled(!cur.isEmpty());
    cutAction->setEnabled(!cur.isEmpty());
    copyAction->setEnabled(!cur.isEmpty());
    QString str = QString("%1 items selected, row %2").arg(cur.count()).arg(
                activePane()->selectionModel()->currentIndex().row());
    statusBar()->clearMessage();
    statusBar()->showMessage(str, -1);
}

void MainWindow::moveTo(const QString& path) {
    QModelIndex index = fileSystemProxyModel->mapFromSource(fileSystemModel->index(path));
    directoryTreeView->selectionModel()->select(index, QItemSelectionModel::Select);
    activePane()->moveTo(path);
}

void MainWindow::slotShowSearchPanel() {
    searchPanel->show();
    searchPanel->raise();
}

void MainWindow::slotShowContextPaneMenu(const QModelIndexList& list, const QPoint& position) {
    if (list.count() != 0)
        contextSelectionMenu->exec(position);
    else
        contextEmptyMenu->exec(position);
}

void MainWindow::slotShowContextDirectoryMenu(const QPoint& position) {
    contextDirectoryMenu->exec(directoryTreeView->mapToGlobal(position));
}

void MainWindow::slotOpenFile() {
    QFileInfo fileInfo = fileSystemModel->fileInfo(activePane()->selectionModel()->currentIndex());
    if (fileInfo.isFile()) {
        manualEditor->loadFile(fileInfo);
        manualEditor->slotShow();
    }
}

void MainWindow::slotOpenDir() {
    QFileInfo fileInfo = fileSystemModel->fileInfo(activePane()->selectionModel()->currentIndex());
    if (!fileInfo.isDir())
        return;
    activePane()->slotDoubleClickedOnEntry(activePane()->selectionModel()->currentIndex());
}

void MainWindow::slotCut() {

    if (!(focusWidget() == activePane()->focusWidget()))
        return;
    selectionList = activePane()->selectionModel()->selectedIndexes();

    if(selectionList.count() == 0) {
        return;
    }
    QApplication::clipboard()->setMimeData(fileSystemModel->mimeData(selectionList));
    pasteAction->setData(true);

    activePane()->selectionModel()->clear();
}

void MainWindow::slotCopy() {

    if (focusWidget() == activePane()->focusWidget()) {
        sourceCopyPath = fileSystemModel->fileInfo(activePane()->focusView()->rootIndex()).absoluteDir().path();
        selectionList = activePane()->selectionModel()->selectedIndexes();
    } else if (focusWidget() == directoryTreeView) {
        selectionList << fileSystemProxyModel->mapToSource(directoryTreeView->currentIndex());
    }

    if (selectionList.count() == 0) {
        return;
    }
    QApplication::clipboard()->setMimeData(fileSystemModel->mimeData(selectionList));
    pasteAction->setData(false);

}


void MainWindow::copyPath(const QString& source, const QString& dist) {
    QDir dir(source);
    if (!dir.exists())
        return;
    QChar sep = QDir::separator();

    if (dist.indexOf(source, 0) != -1) {
        return;
    }

    QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::Files);
    for(const auto & info : list) {
        if (info.isDir() && !QDir(info.absoluteFilePath()).dirName().isEmpty()) {
            dir.mkpath(dist + sep + dir.dirName());
            copyPath(source + sep + QDir(info.absoluteFilePath()).dirName(), dist + sep + dir.dirName());
        } else if (info.isFile() && !QDir(info.absoluteFilePath()).dirName().isEmpty()) {
            QFile::copy(source + sep + QDir(info.absoluteFilePath()).dirName(),
                        dist + sep + dir.dirName() + sep + QDir(info.absoluteFilePath()).dirName());
        }
    }

}

void MainWindow::copyFolders(const QModelIndexList& list, const QString& dist) {
    int i = 0;
    QFileInfo fileInfo = fileSystemModel->fileInfo(list.at(i));
    QString topSelection = fileInfo.absolutePath();
    while (fileInfo.absolutePath() == topSelection && i < list.count()) {
        if (fileInfo.isDir())
            copyPath(fileInfo.absoluteFilePath(), dist);
        fileInfo = fileSystemModel->fileInfo(list.at(i++));
    }
}

void MainWindow::slotPaste() {
    Qt::DropAction cutOrCopy(pasteAction->data().toBool() ? Qt::MoveAction : Qt::CopyAction);
    pasteAction->setEnabled(!pasteAction->data().toBool());
    if (focusWidget() == activePane()->focusWidget()) {
            fileSystemModel->dropMimeData(QApplication::clipboard()->mimeData(), cutOrCopy, 0, 0,
                                      qobject_cast<QAbstractItemView*>(focusWidget())->rootIndex());
            if (!pasteAction->data().toBool()) {
                QFileInfo fileInfo = fileSystemModel->fileInfo(qobject_cast<QAbstractItemView*>(focusWidget())->rootIndex());
                copyFolders(selectionList, fileInfo.absoluteFilePath());
            }
    } else if (focusWidget() == directoryTreeView) {
        bool b = fileSystemModel->dropMimeData(QApplication::clipboard()->mimeData(), cutOrCopy, 0, 0,
                                      fileSystemProxyModel->mapToSource(directoryTreeView->currentIndex()));
        if (!b) {

        }
    }
}

void MainWindow::slotDel() {

    QModelIndexList selectionList;
    bool yesToAll = false;
    bool ok = false;
    bool confirm = true;

    if (!(focusWidget() == activePane()->focusWidget()))
        return;
    selectionList = activePane()->selectionModel()->selectedIndexes();
    if (selectionList.count() == 0)
        return;

    for(int i = 0; i < selectionList.count(); ++i) {

        QFileInfo file(fileSystemModel->filePath(selectionList.at(i)));
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
                ok = fileSystemModel->remove(selectionList.at(i));
            }
        }
        else if(file.isSymLink())
            ok = QFile::remove(file.filePath());
    }

    if(!ok)
        QMessageBox::information(this, tr("Not Deleted"), tr("Some Files Cannot be Deleted."));
}

void MainWindow::slotNewFolder() {

    QAbstractItemView* currentView = activePane()->focusView();
    QModelIndex newDir;
    QString folderName("New Folder");
    int counter = 3;
    while (!newDir.isValid()) {
        newDir = fileSystemModel->mkdir(currentView->rootIndex(), folderName);
        if (newDir.isValid()) {
            currentView->selectionModel()->setCurrentIndex(newDir, QItemSelectionModel::ClearAndSelect);
            currentView->edit(newDir);
        }
        else folderName = (folderName == "New Folder") ? (folderName = "New Folder (2)") :
                                                    (folderName = QString("New Folder (%1)").arg(counter++));
    }

}

void MainWindow::slotNewTxt() {

    QAbstractItemView* currentView = activePane()->focusView();
    QModelIndex ind = currentView->rootIndex();
    QString fileTempName = tr("/New Text Document.txt");
    int counter = 3;
    QFile newFile;
    while (!newFile.isOpen()) {
        newFile.setFileName(fileSystemModel->fileInfo(ind).absoluteFilePath() + fileTempName);
        if (!newFile.exists())
            newFile.open(QFile::ReadWrite);
        else fileTempName = (fileTempName == "/New Text Document.txt") ? fileTempName = "/New Text Document (2).txt" :
                                                    fileTempName = QString("/New Text Document (%1).txt").arg(counter++);
    }
    QModelIndex fileIndex = fileSystemModel->index(newFile.fileName());
    currentView->setCurrentIndex(fileIndex);
    currentView->edit(fileIndex);
    newFile.close();

}

void MainWindow::slotToggleHidden() {
    auto mask = QDir::NoDot | QDir::AllEntries | QDir::System;
    if(hiddenAction->isChecked())
        mask |= QDir::Hidden;
     fileSystemModel->setFilter(mask);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveState(settings);
    QMainWindow::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
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

void MainWindow::saveState(QSettings* settings) const {
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

QSettings* MainWindow::restoreState() {
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

void MainWindow::updateViewActions() {

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

void MainWindow::slotShowAboutCreatorBox() {
    QMessageBox::about(this, tr("About"),
                       tr("<h2>File Manager</h2>"
                          "<p><em>Version 0.4.5</em><br>"
                          "К-26<br>"
                          "made by Alex. Kushnirenko, 2019<br>"));
}

void MainWindow::slotShowParameters() {
    Parameters * parameters = new Parameters(formParametersList(), this);
    connect(parameters, SIGNAL(parametersChanged(const QList<bool>&)), this, SLOT(slotParametersChanged(const QList<bool>&)));
    parameters->exec();
}

void MainWindow::slotParametersChanged(const QList<bool>& list) {
    topToolBar->setVisible(list.at(0));
    statusBar()->setVisible(list.at(1));
}

void MainWindow::slotShowProperties() {
    if (activePane()->selectionModel()->selectedIndexes().count()) {
        QModelIndex index = activePane()->selectionModel()->selectedIndexes().at(0);
        const QFileInfo& fileInfo = fileSystemModel->fileInfo(index);
        Properties properties(fileInfo, this);
        properties.exec();
    }
}

//private methods

QList<QPair<QString, bool>> MainWindow::formParametersList() const {
    QList<QPair<QString, bool>> list;
    list.push_back(QPair{tr("Show View Panel"), topToolBar->isVisible()});
    list.push_back(QPair{tr("Show Status Bar"), statusBar()->isVisible()});
    return list;
}

void MainWindow::createActionsAndMenus() {

    createActions();
    createViewChangeActions();
    createViewActionGroup();

    createMenus();
    createTopToolBar();
    createContextMenus();

}

void MainWindow::createActions() {

    deleteAction = Action::create(QIcon::fromTheme("edit-delete", QIcon(":/Images/Delete.ico")), tr("&Delete"),
                                  "Delete File", QKeySequence::Delete);
    deleteAction->setEnabled(false);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(slotDel()));

    cutAction = Action::create(QIcon::fromTheme("edit-cut", QIcon(":/Images/Cut.png")), tr("&Cut"),
                            "Cut File", QKeySequence::Cut);
    cutAction->setEnabled(false);
    connect(cutAction, SIGNAL(triggered()), this, SLOT(slotCut()));

    copyAction = Action::create(QIcon::fromTheme("edit-copy", QIcon(":/Images/Copy.png")), tr("&Copy"),
                             "Copy File", QKeySequence::Copy);
    copyAction->setEnabled(false);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(slotCopy()));

    pasteAction = Action::create(QIcon::fromTheme("edit-paste", QIcon(":/Images/Paste.png")), tr("Paste"),
                                 "Paste File", QKeySequence::Paste);
    pasteAction->setEnabled(false);
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(slotPaste()));

    newFolderAction = Action::create(QIcon::fromTheme("edit-new-folder", QIcon(":/Images/NewFolder.ico")), tr("&New Folder"),
                                     "Create New Folder", QKeySequence::New);
    connect(newFolderAction, SIGNAL(triggered()), this, SLOT(slotNewFolder()));

    newTxtAction = Action::create(QIcon::fromTheme("edit-new-file", QIcon(":/Images/NewFile.ico")), tr("&New File"),
                                  "Create New File");
    connect(newTxtAction, SIGNAL(triggered()), this, SLOT(slotNewTxt()));

    openAction = Action::create(QIcon(":/Images/OpenFile.ico"), tr("&Open"), "Open Element", QKeySequence::Open);
    connect(openAction, SIGNAL(triggered()), this, SLOT(slotOpenFile()));
    connect(openAction, SIGNAL(triggered()), this, SLOT(slotOpenDir()));

    parametersAction = Action::create(QIcon::fromTheme("parameters-other", QIcon(":/Images/Preferences.ico")), tr("&Parameters"),
                                    "Set Parameters", QKeySequence::Preferences);
    connect(parametersAction, SIGNAL(triggered()), this, SLOT(slotShowParameters()));

    exitAction = Action::create(QIcon::fromTheme("application-exit", QIcon(":/Images/Exit.png")), tr("&Quit"),
                             "Quit", QKeySequence::Quit);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    hiddenAction = Action::create(QIcon::fromTheme("folder-saved-search"), tr("Hidden Files"), "Show Hidden Files");
    hiddenAction->setCheckable(true);
    connect(hiddenAction, SIGNAL(triggered()), this, SLOT(slotToggleHidden()));

    openEditorAction = Action::create(QIcon::fromTheme("edit-new-editor", QIcon(":/Images/Notepad.ico")), tr("&File Editor"),
                                      "Open File Editor");
    connect(openEditorAction, SIGNAL(triggered()), manualEditor, SLOT(slotShow()));

    openSpreadsheetAction = Action::create(QIcon::fromTheme("edit-new-spreadsheet", QIcon(":/Images/Spreadsheet.ico")), tr("&Spreadsheet"),
                                      "Open Spreadsheet");
    connect(openSpreadsheetAction, SIGNAL(triggered()), spreadsheet, SLOT(slotShow()));
    aboutCreatorAction = Action::create(QIcon::fromTheme("help-about", QIcon(":/Images/About.ico")), tr("&About"), "About");
    connect(aboutCreatorAction, SIGNAL(triggered()), this, SLOT(slotShowAboutCreatorBox()));

    propertiesAction = Action::create(QIcon::fromTheme("document-properties", QIcon(":/Images/Properties.ico")), tr("&Properties"),
                                   "Properties", QKeySequence(Qt::CTRL + Qt::Key_R));
    connect(propertiesAction, SIGNAL(triggered()), this, SLOT(slotShowProperties()));
}

void MainWindow::createMenus() {

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

void MainWindow::createViewActionGroup() {
    if (detailViewAction == nullptr || iconViewAction == nullptr || tileViewAction == nullptr)
        return;
    viewActionGroup = new QActionGroup(this);
    viewActionGroup->addAction(detailViewAction);
    viewActionGroup->addAction(iconViewAction);
    viewActionGroup->addAction(tileViewAction);
}

void MainWindow::createTopToolBar() {
    QPushButton* openSearchPanel = new QPushButton("Search");
    openSearchPanel->setFlat(true);
    connect(openSearchPanel, SIGNAL(clicked()), this, SLOT(slotShowSearchPanel()));
    topToolBar = new QToolBar;
    topToolBar->addActions(QList<QAction*>() << detailViewAction << iconViewAction << tileViewAction);
    topToolBar->addSeparator();
    topToolBar->addWidget(openSearchPanel);
    addToolBar(Qt::TopToolBarArea, topToolBar);
}

Pane* MainWindow::createPane() {
    Pane * p = new Pane(fileSystemModel, splitter);
    connect(p, SIGNAL(fileLoaded(const QFileInfo&)), this, SLOT(loadFile(const QFileInfo&)));
    connect(p, SIGNAL(directoryLoaded(const QModelIndex&)), this, SLOT(loadDirectoryTree(const QModelIndex&)));
    connect(p, SIGNAL(contextMenuChanged(const QModelIndexList&, const QPoint&)),
            this, SLOT(slotShowContextPaneMenu(const QModelIndexList&, const QPoint&)));
    return p;
}

void MainWindow::createViewChangeActions() {

    detailViewAction = Action::create(QIcon::fromTheme("view-list-details", QIcon(":/Images/DetailView.ico")), tr("Table"),
                                   "Table View");
    detailViewAction->setCheckable(true);
    connect(detailViewAction, SIGNAL(triggered()), paneSwitcher, SLOT(slotToggleToDetailView()));

    iconViewAction = Action::create(QIcon::fromTheme("view-list-icons", QIcon(":/Images/IconView.ico")), tr("List"),
                                    "List View");
    iconViewAction->setCheckable(true);
    connect(iconViewAction, SIGNAL(triggered()), paneSwitcher, SLOT(slotToggleToIconView()));

    tileViewAction = Action::create(QIcon(":/Images/TileView.png"), tr("Tile"), "Tile View");
    tileViewAction->setCheckable(true);
    connect(tileViewAction, SIGNAL(triggered()), paneSwitcher, SLOT(slotToggleToTileView()));
}

void MainWindow::createContextMenus() {
    createContextSelectionMenu();
    createContextEmptyMenu();
    createContextDirectoryMenu();
}

void MainWindow::createContextSelectionMenu() {
    contextSelectionMenu = new QMenu(this);
    contextSelectionMenu->addAction(openAction);
    contextSelectionMenu->addSeparator();
    contextSelectionMenu->addActions(QList<QAction*>() << detailViewAction << iconViewAction << tileViewAction);
    contextSelectionMenu->addSeparator();
    contextSelectionMenu->addAction(propertiesAction);
    contextSelectionMenu->addSeparator();
    contextSelectionMenu->addActions(QList<QAction*>() << cutAction << copyAction << pasteAction);
    contextSelectionMenu->addSeparator();
    Action *delete2Action = Action::create(QIcon::fromTheme("edit-delete", QIcon(":/Images/Delete.ico")), tr("&Delete"),
                                  "Delete File", QKeySequence::Delete);
    connect(delete2Action, SIGNAL(triggered()), this, SLOT(slotDel()));
    contextSelectionMenu->addAction(delete2Action);
}

void MainWindow::createContextEmptyMenu() {
    contextEmptyMenu = new QMenu(this);
    contextEmptyMenu->addActions(QList<QAction*>() << newTxtAction << newFolderAction);
    contextEmptyMenu->addSeparator();
    contextEmptyMenu->addActions(QList<QAction*>() << detailViewAction << iconViewAction << tileViewAction);
    contextEmptyMenu->addSeparator();
    contextEmptyMenu->addAction(pasteAction);
}

void MainWindow::createContextDirectoryMenu() {
    contextDirectoryMenu = new QMenu(this);
    contextDirectoryMenu->addActions(QList<QAction*>() << copyAction << pasteAction);
}

