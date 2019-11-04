#include <QDebug>

#include "DocumentContainer.h"

DocumentContainer::DocumentContainer() {
    emit containerIsEmpty(cont.isEmpty());
}

auto DocumentContainer::insert(const QString& str, IDocument* doc) -> iterator {
    auto it = cont.insert(str, doc);
    qDebug() << "DocumentContainer::insert(): container size after inserting; " << str << " " << cont.count();
    emit containerIsEmpty(cont.isEmpty());
    return it;
}

int DocumentContainer::remove(const QString& str) {
    int temp = cont.remove(str);
    qDebug() << "DocumentContainer::remove(): container size after removing; " << str << " " << cont.count();
    emit containerIsEmpty(cont.isEmpty());
    return temp;
}

void DocumentContainer::slotReplace(const QString& cur, const QString& prev) {
    qDebug() << "DocumentContainer::slotReplace(): container size after replacing; " << prev
             << " with " << cur << " " << cont.count();
    if (cont.contains(prev)) {
        IDocument* doc = cont.value(prev);
        cont.remove(prev);
        cont.insert(cur, doc);
    }
    emit containerIsEmpty(cont.isEmpty());
}

void DocumentContainer::slotRemove(const QString& str) {
    int temp = cont.remove(str);
    qDebug() << "DocumentContainer::slotRemove(): container size after removing; " << str << " " << cont.count();
    emit containerIsEmpty(cont.isEmpty());
}



