#pragma once

#include <QObject>
#include <QTextEdit>
#include <QString>

#include "ISettings.h"

class IDocument: public QTextEdit {
    Q_OBJECT
public:

    IDocument(QWidget* wgt = nullptr);
    virtual ~IDocument();

    virtual QString fileName() const = 0;
    virtual void setFileName(const QString&) = 0;
    virtual void load(const QString&) = 0;
    virtual void save() = 0;
    virtual void saveAs(const QString&) = 0;

signals:
    void fileNameChanged(const QString& cur, const QString& prev);
    void fileClosed(const QString& fileName);

};
