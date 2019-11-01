
#include "Action.h"
#include "FileManager.h"
#include "Pane.h"
#include "Dialogs/Parameters.h"
#include "Dialogs/Properties.h"
#include "SearchPanel.h"
#include "ProxyModel.h"

FileManager::FileManager(QObject *parent) : QObject(parent), fileSystemModel(new QFileSystemModel()),
        fileSystemProxyModel(new FileSystemModelFilterProxyModel())
{
    fileSystemModel->setFilter(QDir::NoDot | QDir::AllEntries | QDir::System);
    fileSystemModel->setRootPath("");
    fileSystemModel->setReadOnly(false);

    fileSystemProxyModel->setSourceModel(fileSystemModel);
    fileSystemProxyModel->setSortCaseSensitivity(Qt::CaseSensitive);
}

QFileInfo FileManager::fileInfo(const QModelIndex& index) const {
    return fileSystemModel->fileInfo(index);
}

QModelIndex FileManager::index(const QString& str) const {
    return fileSystemModel->index(str);
}

QModelIndex FileManager::mapToSource(const QModelIndex& sourceIndex) const {
    return fileSystemProxyModel->mapToSource(sourceIndex);
}

QModelIndex FileManager::mapFromSource(const QModelIndex& sourceIndex) const {
    return fileSystemProxyModel->mapFromSource(sourceIndex);
}

void FileManager::setFilter(QDir::Filters filters) {
    fileSystemModel->setFilter(filters);
}

QSortFilterProxyModel* FileManager::proxyModel() const {
    return fileSystemProxyModel;
}

QFileSystemModel* FileManager::sourceModel() const {
    return fileSystemModel;
}

bool FileManager::cut(const QModelIndexList& list) {
    if(list.count() == 0) {
        return false;
    }
    selectionList = list;
    QApplication::clipboard()->setMimeData(fileSystemModel->mimeData(selectionList));
    return true;
}

bool FileManager::copy(const QModelIndexList& list) {
    if (list.count() == 0) {
        return false;
    }
    selectionList = list;
    sourceCopyPath = fileInfo(list[0].parent()).absoluteDir().path();
    QApplication::clipboard()->setMimeData(fileSystemModel->mimeData(selectionList));
    return true;
}


void FileManager::copyPath(const QString& source, const QString& dist) {
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

void FileManager::copyFolders(const QModelIndexList& list, const QString& dist) {
    int i = 0;
    QFileInfo fileInfo = fileSystemModel->fileInfo(list.at(i));
    QString topSelection = fileInfo.absolutePath();
    while (fileInfo.absolutePath() == topSelection && i < list.count()) {
        if (fileInfo.isDir())
            copyPath(fileInfo.absoluteFilePath(), dist);
        fileInfo = fileSystemModel->fileInfo(list.at(i++));
    }
}

void FileManager::paste(const QModelIndex& rootIndex, Qt::DropAction action) {
    fileSystemModel->dropMimeData(QApplication::clipboard()->mimeData(), action, 0, 0, rootIndex);
    if (action == Qt::DropAction::CopyAction) {
        QFileInfo fileInfo = fileSystemModel->fileInfo(rootIndex);
        copyFolders(selectionList, fileInfo.absoluteFilePath());
    }
}

void FileManager::del(const QModelIndexList& list) {
    for (const auto & it: list)
        del(it);
}
bool FileManager::del(const QModelIndex& index) {
    if (fileInfo(index).isSymLink())
        return QFile::remove(fileInfo(index).filePath());
    else
        return fileSystemModel->remove(index);
}

QModelIndex FileManager::newFolder(const QModelIndex& rootIndex) {

    QModelIndex newDir;
    QString folderName("New Folder");
    int counter = 3;
    while (!newDir.isValid()) {
        newDir = fileSystemModel->mkdir(rootIndex, folderName);
        folderName = (folderName == "New Folder") ? (folderName = "New Folder (2)") :
                                                    (folderName = QString("New Folder (%1)").arg(counter++));
    }
    return newDir;
}

QModelIndex FileManager::newTxt(const QModelIndex& rootIndex) {

    QFile newFile;
    QString fileTempName = tr("/New Text Document.txt");
    int counter = 3;
    while (!newFile.isOpen()) {
        newFile.setFileName(fileSystemModel->fileInfo(rootIndex).absoluteFilePath() + fileTempName);
        if (!newFile.exists())
            newFile.open(QFile::ReadWrite);
        else fileTempName = (fileTempName == "/New Text Document.txt") ? fileTempName = "/New Text Document (2).txt" :
                                                    fileTempName = QString("/New Text Document (%1).txt").arg(counter++);
    }
    newFile.close();
    return fileSystemModel->index(newFile.fileName());
}

