#pragma once

#include <QDialog>

class QDialogButtonBox;
class QGroupBox;
class QCheckBox;

class Parameters : public QDialog {
    Q_OBJECT
public:
    explicit Parameters(const QList<QPair<QString, bool>>&, QWidget *parent = nullptr);

private:
    QList<QCheckBox*> checkBoxList;

private slots:
    void slotAccept();
    void slotReject();

signals:
    void parametersChanged(const QList<bool>&);

};
