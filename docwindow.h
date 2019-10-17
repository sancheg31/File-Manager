#pragma once

#include <QTextEdit>
#include <QList>
#include <QFileInfo>

class DocWindow: public QTextEdit {
Q_OBJECT
public:
    DocWindow(QWidget * = nullptr);
    const QString& getFileName() const;

protected:
    void closeEvent(QCloseEvent*);

private:
    QString fileName;

signals:
    void close(const QString&);
    void fileNameChanged(const QString&, const QString&);

public slots:
    void slotLoad(const QString&);
    void slotSave();
    void slotSaveAs();

};

