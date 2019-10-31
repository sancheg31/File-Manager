#include <QtGui>
#include <QtWidgets>

#include "spreadsheet.h"
#include "SpreadSheetCompare.h"
#include "SpreadsheetWindow.h"

#include "Dialogs/FindDialog.h"
#include "Dialogs/GoToCellDialog.h"
#include "Dialogs/SortDialog.h"


SpreadsheetWindow::SpreadsheetWindow(QWidget *parent) : QMainWindow(parent), sheet(new Spreadsheet)
{
    setWindowIcon(QIcon(":/Images/Spreadsheet.png"));

    QStringList headers;
    for (char i = 'A'; i != 'Z'; ++i)
        headers << QString(i);

   // tbl = new QTableWidget;
    sheet->setColumnCount(26);
    sheet->setRowCount(30);
    setCentralWidget(sheet);

    sheet->setHorizontalHeaderLabels(headers);

    createActions();
    createMenus();
    createContextMenu();
    createToolBars();
    createStatusBar();

    settings = restoreState();
    findDialog = nullptr;

    setCurrentFile("");

}

SpreadsheetWindow::~SpreadsheetWindow() {

}

bool SpreadsheetWindow::okToContinue() {
    if (isWindowModified()) {
        int res = QMessageBox::warning(this, tr("Spreadsheet"),
                  tr("The document has been modified.\n Do you want to save changes?"),
                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (res == QMessageBox::Yes)
            return slotSave();
        else if (res == QMessageBox::Cancel)
            return false;
    }
    return true;
}

QSettings* SpreadsheetWindow::restoreState(){
    QSettings* settings = new QSettings("Alexander Kushnirenko", "spreadsheet");
    restoreGeometry(settings->value("Geometry").toByteArray());
    recentFile = settings->value("RecentFiles").toStringList();
    updateRecentFileActions();
    showGridAction->setChecked(settings->value("ShowGrid", true).toBool());
    autoRecalculateAction->setChecked(settings->value("AutoRecalculate", true).toBool());
    return settings;
}

void SpreadsheetWindow::saveState(QSettings* settings) const {
    settings->setValue("Geometry", saveGeometry());
    settings->setValue("RecentFiles", recentFile);
    settings->setValue("ShowGrid", showGridAction->isChecked());
    settings->setValue("AutoRecalculate", autoRecalculateAction->isChecked());
}

void SpreadsheetWindow::slotNewFile(){
    if (okToContinue()) {
        sheet->clear();
        setCurrentFile("");
    }
}

void SpreadsheetWindow::slotOpen(){
    if (okToContinue()) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open Spreadsheet"), ".", tr("Spreadsheet files (*.sp)"));
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool SpreadsheetWindow::slotSave(){
    if (curFile.isEmpty()) {
        return slotSaveAs();
    } else {
        return saveFile(curFile);
    }
}

bool SpreadsheetWindow::slotSaveAs(){
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Spreadsheet"), ".", tr("Spreadsheet files (*.sp)"));
    if (fileName.isEmpty())
        return false;
    return saveFile(fileName);
}

void SpreadsheetWindow::slotFind(){
    if (!findDialog) {
        findDialog = new FindDialog(this);
        connect(findDialog, SIGNAL(findNext(const QString&, Qt::CaseSensitivity)),
                sheet, SLOT(findNext(const QString&, Qt::CaseSensivity)));
        connect(findDialog, SIGNAL(findPrevious(const QString&, Qt::CaseSensitivity)),
                sheet, SLOT(findPrevious(const QString&, Qt::CaseSensivity)));
    }
    findDialog->setModal(false);
    findDialog->show();
    findDialog->raise();
    findDialog->activateWindow();
}

void SpreadsheetWindow::slotGoToCell(){
    GoToCellDialog * dialog = new GoToCellDialog(this);
    if (dialog->exec()) {
        QString str = dialog->lineEdit->text().toUpper();
        sheet->setCurrentCell(str.mid(1).toInt()-1, str[0].unicode() - 'A');
    }
    delete dialog;
}


void SpreadsheetWindow::slotSort(){
    SortDialog dialog(this);
    QTableWidgetSelectionRange range = sheet->selectedRange();
    dialog.setColumnRange('A' + range.leftColumn(), 'A' + range.rightColumn());
    if (dialog.exec()) {
        SpreadsheetCompare compare;
        compare.keys[0] = dialog.primaryColumnCombo->currentIndex();
        compare.keys[1] = dialog.secondaryColumnCombo->currentIndex() - 1;
        compare.keys[2] = dialog.tertiaryColumnCombo->currentIndex() - 1;
        compare.ascending[0] = (dialog.primaryOrderCombo->currentIndex() == 0);
        compare.ascending[1] = (dialog.secondaryOrderCombo->currentIndex() == 0);
        compare.ascending[2] = (dialog.tertiaryOrderCombo->currentIndex() == 0);
        sheet->sort(compare);
    }
}

void SpreadsheetWindow::slotShowGrid(){

}

void SpreadsheetWindow::slotAbout(){

}

void SpreadsheetWindow::slotAboutQt(){

}

void SpreadsheetWindow::slotShow() {
    show();
    raise();
}

void SpreadsheetWindow::slotOpenRecentFile(){
    if (okToContinue()) {
        QAction* action = qobject_cast<QAction*>(sender());
        if (action) {
            loadFile(action->data().toString());
        }
    }
}

void SpreadsheetWindow::slotUpdateStatusBar(){
    locationLabel->setText(sheet->currentLocation());
    formulaLabel->setText(sheet->currentFormula());
}

void SpreadsheetWindow::slotSpreadsheetModified(){
    setWindowModified(true);
    slotUpdateStatusBar();
}

void SpreadsheetWindow::closeEvent(QCloseEvent * e) {
    saveState(settings);
    QMainWindow::closeEvent(e);
}

//-------------------------------------------------

bool SpreadsheetWindow::loadFile(const QString& fileName) {
    if (!sheet->readFile(fileName)) {
        statusBar()->showMessage(tr("Loading canceled"), 4000);
        return false;
    }
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File Loaded"), 4000);
    return true;
}

bool SpreadsheetWindow::saveFile(const QString& fileName) {
    if (!sheet->writeFile(fileName)) {
        statusBar()->showMessage(tr("Saving canceled"), 4000);
        return false;
    }
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File Saved"), 4000);
    return true;
}

void SpreadsheetWindow::setCurrentFile(const QString &fileName){
    curFile = fileName;
    setWindowModified(false);
    QString showName = "Untitled";
    if (!curFile.isEmpty()) {
        showName = strippedName(curFile);
        recentFile.removeAll(curFile);
        recentFile.prepend(curFile);
        updateRecentFileActions();
    }
    setWindowTitle(tr("%1[*] - %2").arg(showName).arg(tr("Spreadsheet")));
}

QString SpreadsheetWindow::strippedName(const QString &fullFileName){
    return QFileInfo(fullFileName).fileName();
}

void SpreadsheetWindow::updateRecentFileActions() {

    QMutableStringListIterator i(recentFile);
    while (i.hasNext()) {
        if (!QFile::exists(i.next()))
            i.remove();
    }

    for(int j = 0; j < MaxRecentFiles; ++j){
            if(j < recentFile.count()) {
                QString text = tr("&%1 %2").arg(j + 1).arg(strippedName(recentFile[j]));
                recentFileActions[j]->setText(text);
                recentFileActions[j]->setData(recentFile[j]);
                recentFileActions[j]->setVisible(true);
            } else {
                recentFileActions[j]->setVisible(false);
            }
    }
    separatorAction->setVisible(!recentFile.isEmpty());

}

void SpreadsheetWindow::createActions(){

    newAction = new QAction(QIcon(":/Images/NewFile.png"), tr("&New"), this);
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip("Create Spreadsheet File");
    connect(newAction, SIGNAL(triggered()), this, SLOT(slotNewFile()));

    openAction = new QAction(QIcon(":/Images/Open.png"), tr("&Open"), this);
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip("Open Spreadsheet File");
    connect(openAction, SIGNAL(triggered()), this, SLOT(slotOpen()));

    saveAction = new QAction(QIcon(":/Images/Save.png"), tr("&Save"), this);
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip("Save Spreadsheet File");
    connect(saveAction, SIGNAL(triggered()), this, SLOT(slotSave()));


    saveAsAction = new QAction(QIcon(":/Images/SaveAs.png"), tr("&Save As..."), this);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    saveAsAction->setStatusTip("Save Spreadsheet File As");
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(slotSaveAs()));

    for(int i = 0; i < MaxRecentFiles; i++){
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()), this, SLOT(slotOpenRecentFile()));
    }

    exitAction = new QAction(QIcon(":/Images/Exit.png"), tr("E&xit"), this);
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setStatusTip("Exit the application");
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    cutAction = new QAction(QIcon(":/Images/Cut.png"), tr("Cu&t"), this);
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setStatusTip("Cut");
    connect(cutAction, SIGNAL(triggered()), sheet, SLOT(cut()));


    copyAction = new QAction(QIcon(":/Images/Copy.png"), tr("&Copy"), this);
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setStatusTip("Copy");
    connect(copyAction, SIGNAL(triggered()), sheet, SLOT(copy()));

    pasteAction = new QAction(QIcon(":/Images/Paste.png"), tr("&Paste"), this);
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setStatusTip("Paste");
    connect(pasteAction, SIGNAL(triggered()), sheet, SLOT(paste()));

    deleteAction = new QAction(QIcon(":/Images/Delete.png"), tr("Delete"), this);
    deleteAction->setShortcut(QKeySequence::Delete);
    deleteAction->setStatusTip("Delete");
    connect(deleteAction, SIGNAL(triggered()), sheet, SLOT(del()));

    selectAction = new QAction(QIcon(":/Images/Select.png"), tr("Select"), this);
    selectAction->setStatusTip("Select Cells");

    rowAction = new QAction(QIcon(":/Images/SelectRow.png"), tr("&Row"), this);
    connect(rowAction, SIGNAL(triggered()), sheet, SLOT(selectCurrentRow()));

    columnAction = new QAction(QIcon(":/Images/SelectColumn.png"), tr("&Column"), this);
    connect(columnAction, SIGNAL(triggered()), sheet, SLOT(selectCurrentColumn()));

    allAction = new QAction(QIcon(":/Images/SelectAll.png"), tr("&All"), this);
    allAction->setShortcut(QKeySequence::SelectAll);
    allAction->setStatusTip(tr("Select All Cells"));
    connect(allAction, SIGNAL(triggered()), sheet, SLOT(selectAll()));

    findAction = new QAction(QIcon(":/Images/Find.png"), tr("&Find"), this);
    findAction->setShortcut(QKeySequence::Find);
    findAction->setStatusTip("Find Cell");
    connect(findAction, SIGNAL(triggered()), this, SLOT(slotFind()));

    goToCellAction = new QAction(QIcon(":/Images/GoTo.png"), tr("&Go to cell..."), this);
    goToCellAction->setShortcut(tr("Ctrl+G"));
    goToCellAction->setStatusTip("Go to Cell...");
    connect(goToCellAction, SIGNAL(triggered()), this, SLOT(slotGoToCell()));

    recalculateAction = new QAction(QIcon(":/Images/Recalculate.png"), tr("&Recalculate"), this);
    recalculateAction->setShortcut(tr("F9"));
    connect(recalculateAction, SIGNAL(triggered()), sheet, SLOT(recalculate()));

    sortAction = new QAction(QIcon(":/Images/Sort.png"), tr("&Sort..."), this);
    sortAction->setStatusTip("Invoke Sort Panel");
    connect(sortAction, SIGNAL(triggered()), this, SLOT(slotSort()));

    showGridAction = new QAction(QIcon(":/Images/Grid.png"), tr("&Show Grid"), this);
    showGridAction->setCheckable(true);
    showGridAction->setChecked(sheet->showGrid());
    showGridAction->setStatusTip(tr("Show or Hide Grid"));
    connect(showGridAction, SIGNAL(toggled(bool)), sheet, SLOT(setShowGrid(bool)));

    autoRecalculateAction = new QAction(QIcon(":/Images/Recalculate.png"), tr("&Auto-Recalculate"), this);
    autoRecalculateAction->setCheckable(true);
    autoRecalculateAction->setChecked(sheet->autoRecalculate());
    autoRecalculateAction->setStatusTip("Auto Recalculation Data in Table");
    connect(autoRecalculateAction, SIGNAL(toggled(bool)), sheet, SLOT(setAutoRecalculate(bool)));

    aboutAction = new QAction(QIcon(":/Images/About.png"), tr("&About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(slotAbout()));

}

void SpreadsheetWindow::createMenus(){

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addActions(QList<QAction*>() << newAction << openAction << saveAction << saveAsAction);
    separatorAction = fileMenu->addSeparator();
    for(int i = 0; i < MaxRecentFiles; i++)
        fileMenu->addAction(recentFileActions[i]);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addActions(QList<QAction*>() << cutAction << copyAction << pasteAction << deleteAction);

    selectSubMenu = editMenu->addMenu(tr("&Select"));
    selectSubMenu->addActions(QList<QAction*>() << rowAction << columnAction << allAction);
    editMenu->addSeparator();
    editMenu->addActions(QList<QAction*>() << findAction << goToCellAction);

    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addActions(QList<QAction*>() << recalculateAction << sortAction);

    optionsMenu = menuBar()->addMenu(tr("&Options"));
    optionsMenu->addActions(QList<QAction*>() << showGridAction << autoRecalculateAction);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addActions(QList<QAction*>() << aboutAction);

}

void SpreadsheetWindow::createContextMenu(){
    sheet->addActions(QList<QAction*>() << cutAction << copyAction << pasteAction);
    sheet->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void SpreadsheetWindow::createToolBars(){
    fileToolBar = addToolBar(tr("&File"));
    fileToolBar->addActions(QList<QAction*>() << newAction << openAction << saveAction);

    editToolBar = addToolBar(tr("&Edit"));
    editToolBar->addActions(QList<QAction*>() << cutAction << copyAction << pasteAction);
    editToolBar->addSeparator();
    editToolBar->addActions(QList<QAction*>() << findAction << goToCellAction);
}

void SpreadsheetWindow::createStatusBar(){
    locationLabel = new QLabel(" W999 ");
    locationLabel->setAlignment(Qt::AlignHCenter);
    locationLabel->setMinimumSize(locationLabel->sizeHint());

    formulaLabel = new QLabel;
    formulaLabel->setIndent(3);
    statusBar()->addWidget(locationLabel);
    statusBar()->addWidget(formulaLabel, 1);

    connect(sheet, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(slotUpdateStatusBar()));
    connect(sheet, SIGNAL(modified()), this, SLOT(slotSpreadsheetModified()));

    slotUpdateStatusBar();
}
















