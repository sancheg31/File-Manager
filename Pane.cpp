
#include "Pane.h"

Pane::Pane(QFileSystemModel* model, QWidget* parent) : QFrame (parent), treeView(new TreeView(this)),
    listView(new ListView(this)), tableView(new TableView(this)), pathLineEdit(new QLineEdit)
{

    fileModel = model;
    connect(pathLineEdit, SIGNAL(editingFinished()), this, SLOT(slotPathLineEditChanged()));
    treeView->setModel(fileModel);
    listView->setModel(fileModel);
    tableView->setModel(fileModel);
    listView->setSelectionModel(treeView->selectionModel());
    tableView->setSelectionModel(treeView->selectionModel());

    connect(treeView, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotDoubleClickedOnEntry(const QModelIndex&)));
    connect(treeView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotShowContextMenu(const QPoint&)));

    connect(listView, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotDoubleClickedOnEntry(const QModelIndex&)));
    connect(listView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotShowContextMenu(const QPoint&)));

    connect(tableView, SIGNAL(activated(const QModelIndex&)), this, SLOT(slotDoubleClickedOnEntry(const QModelIndex&)));
    connect(tableView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotShowContextMenu(const QPoint&)));

    stackedWidget = new QStackedWidget();
    stackedWidget->insertWidget(TreeViewMode, treeView);
    stackedWidget->insertWidget(ListViewMode, listView);
    stackedWidget->insertWidget(TableViewMode, tableView);

    QVBoxLayout* vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->addWidget(pathLineEdit);
    vBoxLayout->addWidget(stackedWidget);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->setSpacing(0);
    setLayout(vBoxLayout);
    setFrameStyle(QFrame::Box | QFrame::Plain);

}

QWidget* Pane::focusWidget() const {
    return stackedWidget->focusWidget();
}

QAbstractItemView* Pane::focusView() const {
    return qobject_cast<QAbstractItemView*>(focusWidget());
}

int Pane::focusViewIndex() const {
    return stackedWidget->currentIndex();
}

void Pane::setFocusViewIndex(int i) {
    stackedWidget->setCurrentIndex(i);
}

QLineEdit* Pane::pathLine() const {
    return pathLineEdit;
}

QItemSelectionModel* Pane::selectionModel() const {
    return treeView->selectionModel();
}

void Pane::setAlternatingRowColors(bool b) {
    for (int i = 0; i < stackedWidget->count(); ++i)
        qobject_cast<QAbstractItemView*>(stackedWidget->widget(i))->setAlternatingRowColors(b);
}

void Pane::slotDoubleClickedOnEntry(const QFileInfo& fileInfo) {

    Qt::KeyboardModifiers keybMod = QApplication::keyboardModifiers();
    if(keybMod == Qt::ControlModifier || keybMod == Qt::ShiftModifier)
        return;

    if(fileInfo.isDir()) {
        moveTo(fileInfo.absoluteFilePath());
    } else if (fileInfo.isFile() && fileInfo.absoluteFilePath().endsWith(".txt")) {
        emit fileLoaded(fileInfo);
    } else {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    }

}

void Pane::slotDoubleClickedOnEntry(const QModelIndex& index) {
    QFileInfo fileInfo(fileModel->filePath(index));
    slotDoubleClickedOnEntry(fileInfo);
}

void Pane::moveTo(const QString& path) {
    pathLineEdit->setText(path);
    QModelIndex index(fileModel->index(path));
    for (int i = 0; i < stackedWidget->count(); ++i)
        qobject_cast<QAbstractItemView*>(stackedWidget->widget(i))->setRootIndex(index);
    emit directoryLoaded(index);
}

void Pane::setViewTo(ViewMode viewMode) {
    stackedWidget->setCurrentIndex(viewMode);
}


void Pane::slotShowContextMenu(const QPoint& position) {
    emit contextMenuChanged(selectionModel()->selectedIndexes(), listView->mapToGlobal(position));
}

void Pane::slotPathLineEditChanged() {
    QFileInfo file(pathLineEdit->text());
    if(file.isDir())
        moveTo(pathLineEdit->text());
    else
        pathLineEdit->setText(fileModel->filePath(treeView->rootIndex()));
}

