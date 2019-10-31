#include <QDebug>

#include "DocumentContainer.h"

DocumentContainer::DocumentContainer() {
    emit containerIsEmpty(isEmpty());
}

auto DocumentContainer::insert(const QString& str, IDocument* doc) -> iterator {
    auto it = QHash<QString, IDocument*>::insert(str, doc);
    qDebug() << "DocumentContainer::insert(): container size after inserting; " << str << " " << count();
    emit containerIsEmpty(isEmpty());
    return it;
}

void DocumentContainer::slotReplace(const QString& cur, const QString& prev) {
    qDebug() << "DocumentContainer::slotReplace(): container size after replacing; " << prev
             << " with " << cur << " " << count();
    if (contains(prev)) {
        IDocument* doc = value(prev);
        remove(prev);
        insert(cur, doc);
    }
    emit containerIsEmpty(isEmpty());
}

void DocumentContainer::slotRemove(const QString& str) {
    int temp = remove(str);
    qDebug() << "DocumentContainer::slotRemove(): container size after removing; " << str << " " << count();
    emit containerIsEmpty(isEmpty());
}



