#pragma once

#include <QDialog>
#include <QVBoxLayout>

class QFormLayout;
class QFileInfo;

class Properties : public QDialog {
    Q_OBJECT
public:
    explicit Properties(const QFileInfo& fileInfo, QWidget* parent = nullptr);
    explicit Properties(QWidget* parent = nullptr);

protected:
    QFormLayout* formLayout(const QFileInfo &) const;
    QString detFileSizeNotation(int64_t) const;

private slots:
    void slotAccept();
    void slotReject();

};
