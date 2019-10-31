#pragma once

#include <QTextEdit>
#include <QList>
#include <QFileInfo>

#include "IDocument.h"

class DocWindow: public IDocument {
Q_OBJECT
public:
    DocWindow(QWidget * = nullptr);

    QString fileName() const;

    void load(const QString&);
    void save();
    void saveAs(const QString&);

protected:
    void closeEvent(QCloseEvent*);

private:
    QString fName;

protected slots:
    void slotLoad(const QString&);
    void slotSave();
    void slotSaveAs(const QString&);

};

