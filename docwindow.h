#pragma once

#include <QTextEdit>
#include <QList>
#include <QFileInfo>

#include "Interfaces/IDocument.h"

class DocWindow: public IDocument {
Q_OBJECT
public:
    DocWindow(QWidget * = nullptr);

    QString fileName() const;
    void setFileName(const QString&);

    void load(const QString&);
    void save();
    void saveAs(const QString&);

    State state() const { return st; }
    void setState(State state) { st = state; }

protected:
    void closeEvent(QCloseEvent*);

private:
    State st;
    QString fName;

protected slots:
    void slotLoad(const QString&);
    void slotSave();
    void slotSaveAs(const QString&);
    void slotTextChanged();

};

