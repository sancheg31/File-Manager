#pragma once

#include <QMap>
#include <QPair>
#include <QHash>

#include "IDocument.h"

class DocumentContainer: public QObject, public QHash<QString, IDocument*> {
Q_OBJECT
public:
    DocumentContainer();

    iterator insert(const QString&, IDocument*);

public slots:
    void slotRemove(const QString&);
    void slotReplace(const QString&, const QString&);

signals:
    void containerIsEmpty(bool);
    void documentRemoved(IDocument*);

};
