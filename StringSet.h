#pragma once

#include <QObject>
#include <QSet>
#include <QList>
#include <QString>
#include <type_traits>

class StringSet: public QObject {
    Q_OBJECT
public:
    StringSet(QObject * obj = nullptr);


    QSet<QString>::iterator insert(const QString&);
    QSet<QString>::iterator replace(const QString&, const QString&);
    bool remove(const QString&);

    QList<QString> toList() const;
    bool contains(const QString&) const;
    bool isEmpty() const;

private:
    QSet<QString> set;

private slots:
    void slotInsert(const QString&);
    void slotRemove(const QString&);
    void slotReplace(const QString&, const QString&);
signals:
    void setChanged();
    void setIsEmpty(bool);
};
















