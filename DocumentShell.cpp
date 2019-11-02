#include "DocumentShell.h"

#include <QtWidgets>

DocumentShell::DocumentShell(IDocument *document, QObject * obj): QObject(obj) {
    document = doc;
}

IDocument* DocumentShell::clone() const {
    return doc->clone();
}

IDocument* DocumentShell::document() const {
    return doc;
}

QString DocumentShell::fileName() const {
    return doc->fileName();
}

void DocumentShell::setFileName(const QString & str) {
    doc->setFileName(str);
}

void DocumentShell::load(const QString& str) {
    doc->load(str);
}

void DocumentShell::save() {
    doc->save();
}

void DocumentShell::saveAs(const QString& str) {
    setFileName(str);
    save();
}

void DocumentShell::slotSave() {
    save();
}

void DocumentShell::slotSaveAs(const QString& str) {
    saveAs(str);
}
void DocumentShell::slotLoad(const QString& str) {
    load(str);
}






