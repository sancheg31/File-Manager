#pragma once

#include <QMap>
#include <QPair>
#include <QHash>

#include "Interfaces/IDocument.h"

class DocumentContainer: public QObject {
Q_OBJECT
public:
    using iterator = QHash<QString, IDocument*>::iterator;
    using const_iterator = QHash<QString, IDocument*>::const_iterator;

    DocumentContainer();

    iterator insert(const QString&, IDocument*);
    int remove(const QString&);

    bool isEmpty() const { return cont.isEmpty(); }
    int count() const { return cont.count(); }
    bool contains(const QString& str) const { return cont.contains(str); }
    IDocument* value(const QString& str) const { return cont.value(str); }

    iterator begin() { return cont.begin(); }
    iterator end() { return cont.end(); }
    const_iterator begin() const { return cont.begin(); }
    const_iterator end() const { return cont.end(); }

public slots:
    void slotRemove(const QString&);
    void slotReplace(const QString&, const QString&);

private:
    QHash<QString, IDocument*> cont;

signals:
    void containerIsEmpty(bool);
    void documentRemoved(IDocument*);

};
