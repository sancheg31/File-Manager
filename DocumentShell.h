#pragma once

#include <QTextEdit>
#include <QList>
#include <QFileInfo>

#include "IPrototype.h"
#include "IDocument.h"
#include "IDocumentShell.h"

class DocumentShell: public QObject, public IDocumentShell {
Q_OBJECT
public:

    DocumentShell(IDocument* ,QObject * = nullptr);

    IDocument* clone() const;
    IDocument* document() const;

    QString fileName() const;
    void setFileName(const QString&);

    void load(const QString&);
    void save();
    void saveAs(const QString&);

private:
    IDocument* doc;

protected slots:
    void slotLoad(const QString&);
    void slotSave();
    void slotSaveAs(const QString&);

signals:
    void fileNameChanged(const QString& cur, const QString& prev);
    void fileClosed(const QString& fileName);
};

