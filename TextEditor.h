#pragma once

#include <QtWidgets>

#include "DocumentContainer.h"
#include "DocWindow.h"

#include "IDocument.h"
#include "ISettings.h"

class TextEditor: public QObject, public ISettings {
Q_OBJECT
public:
    TextEditor(QObject * obj = nullptr);

    IDocument* activeDocument() const;
    IDocument* document(const QString&) const;
    bool setActiveDocument(IDocument *);
    const DocumentContainer& getDocuments() const;
    bool isNewFile() const;

    QSettings* restoreState();
    void saveState(QSettings*) const;

public slots:
    void loadFile(const QFileInfo&);
    void loadFile(const QString&, const QString&);

    void undo();
    void cut();
    void copy();
    void paste();
    void del();
    void selectAll();
    void setDateAndTime(QDateTime);
    void setFont(QFont);

    bool newDoc();
    bool load(const QString&);
    bool saveAs(const QString&);
    bool save();

private:

    void loadFiles(const QStringList&);
    void loadFiles(const QList<QFileInfo>&);

    IDocument* createNewDocument();

    IDocument* actDoc;
    QSettings* settings;
    DocumentContainer documents;

};

