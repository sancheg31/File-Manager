#pragma once

#include <QTableWidgetItem>

class Cell : public QTableWidgetItem {
public:
    Cell();

    QTableWidgetItem *clone() const;
    QVariant value() const;
    void setData(int role, const QVariant &value);
    QVariant data(int role) const;
    void setFormula(const QString &formula);
    QString formula() const;
    void setDirty();

private:

    mutable QVariant cachedValue;
    mutable bool modified;
};
