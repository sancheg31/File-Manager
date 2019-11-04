#include "DocWindow.h"

#include <QtWidgets>

DocWindow::DocWindow(QWidget * wgt): IDocument(wgt), st(State::Opened), fName("") {
    createStandardContextMenu();
    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
}

QString DocWindow::fileName() const {
    return fName == "" ? windowTitle() : fName;
}

void DocWindow::setFileName(const QString & str) {
    emit fileNameChanged(str, fileName());
    setWindowTitle(str);
    fName = str;
}

void DocWindow::closeEvent(QCloseEvent * event) {
    qDebug() << (int)st;
    emit fileClosed(fileName());
    if (st != State::Modified) {
        qDebug() << (int)st;
        event->accept();
        QTextEdit::closeEvent(event);
    } else {
        event->ignore();
    }
}

void DocWindow::load(const QString& str) {
    if (str.isEmpty())
        return;
    QFile file(str);
    if (file.open(QIODevice::ReadOnly)) {
         QTextStream stream(&file);
         setPlainText(stream.readAll());
         file.close();
         setFileName(str);
         st = State::Saved;
    }
}

void DocWindow::save() {

    QFile file(fName.endsWith(".txt") ? fName : fName + tr(".txt"));
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream(&file) << toPlainText();
        file.close();
        st = State::Saved;
    }
}

void DocWindow::saveAs(const QString& str) {
    setFileName(str);
    save();
}

void DocWindow::slotSave() {
    save();
}

void DocWindow::slotSaveAs(const QString& str) {
    saveAs(str);
}
void DocWindow::slotLoad(const QString& str) {
    load(str);
}

void DocWindow::slotTextChanged() {
    st = State::Modified;
}







