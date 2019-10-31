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
    TextEditor(const QList<QFileInfo>&, QObject * wgt = nullptr);
    TextEditor(const QStringList&, QObject * wgt = nullptr);

    IDocument* activeDocument() const;
    IDocument* document(const QString&) const;
    bool setActiveDocument(IDocument *);
    const DocumentContainer& getDocuments() const;
    bool isNewFile() const;

    virtual QSettings* restoreState() override;
    virtual void saveState(QSettings*) const override;

public slots:
    void loadFile(const QFileInfo&);
    void loadFile(const QString&, const QString&);

    void undo();
    void cut();
    void copy();
    void paste();
    void del();
    void selectAll();
    void dateAndTime();
    void font();

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

