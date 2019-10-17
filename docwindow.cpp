#include "DocWindow.h"

#include <QtWidgets>

DocWindow::DocWindow(QWidget * wgt): QTextEdit(wgt) {
    setWindowIcon(QIcon(":/Images/Notepad.ico"));
    createStandardContextMenu();
}

const QString& DocWindow::getFileName() const {
    return fileName;
}

void DocWindow::closeEvent(QCloseEvent * event) {
    emit close(windowTitle());
    QTextEdit::closeEvent(event);
}

void DocWindow::slotLoad(const QString& str) {
    if (str.isEmpty())
        return;
    QFile file(str);
    if (file.open(QIODevice::ReadOnly)) {
         QTextStream stream(&file);
         setPlainText(stream.readAll());
         file.close();

         setWindowTitle(str);
         emit fileNameChanged(str, fileName);
         fileName = str;
    }
}

void DocWindow::slotSave() {

    if (fileName.isEmpty()) {
        slotSaveAs();
        return;
    }

    QFile file(fileName.endsWith(".txt") ? fileName : fileName + tr(".txt"));
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream(&file) << toPlainText();
        file.close();
    }
}

void DocWindow::slotSaveAs() {
    QString str = QFileDialog::getSaveFileName(nullptr, tr("Save File"), tr("D:/"), tr("Text Files (*.txt)"));
    if (!str.isEmpty()) {
        emit fileNameChanged(str, fileName);
        setWindowTitle(str);
        fileName = str;
        slotSave();
    }
}







