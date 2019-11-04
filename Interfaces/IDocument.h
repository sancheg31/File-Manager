#pragma once

#include <QObject>
#include <QTextEdit>
#include <QString>

class IDocument: public QTextEdit {
    Q_OBJECT
public:

    enum class State { Opened = 0, Modified = 1, Saved = 2, Closed = 4 };
    IDocument(QWidget* wgt = nullptr);
    virtual ~IDocument();

    virtual State state() const = 0;
    virtual void setState(State) = 0;
    virtual QString fileName() const = 0;
    virtual void setFileName(const QString&) = 0;
    virtual void load(const QString&) = 0;
    virtual void save() = 0;
    virtual void saveAs(const QString&) = 0;

signals:
    void fileNameChanged(const QString& cur, const QString& prev);
    void fileClosed(const QString& fileName);

};
