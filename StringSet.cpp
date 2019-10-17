#include <QDebug>

#include "StringSet.h"


StringSet::StringSet(QObject * obj): QObject(obj) {
    emit setIsEmpty(set.isEmpty());
}

bool StringSet::isEmpty() const {
    return set.isEmpty();
}

bool StringSet::contains(const QString& str) const {
    return set.contains(str);
}

QList<QString> StringSet::toList() const {
    QList<QString> list;
    for (const auto & x: set)
        list.push_back(x);
    return list;
}

bool StringSet::remove(const QString& str) {
    bool b = set.remove(str);
    if (b) {
        emit setChanged();
        emit setIsEmpty(set.isEmpty());
    }
    return b;
}

QSet<QString>::iterator StringSet::insert(const QString& str) {
    auto it = set.insert(str);
    emit setChanged();
    emit setIsEmpty(set.isEmpty());
    return it;
}

QSet<QString>::iterator StringSet::replace(const QString& cur, const QString& prev) {
    set.remove(prev);
    auto it = set.insert(cur);
    emit setChanged();
    emit setIsEmpty(set.isEmpty());
    return it;
}

//slots

void StringSet::slotInsert(const QString& str) {
    insert(str);
}

void StringSet::slotReplace(const QString& cur, const QString& prev) {
    replace(cur, prev);
}
void StringSet::slotRemove(const QString& str) {
    remove(str);
}






