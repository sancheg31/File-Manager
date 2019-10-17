#pragma once

#include <QAction>
#include <QIcon>
#include <QString>

class Action: public QAction {
public:
    Action(QObject* = nullptr);
    Action(const QIcon& icon, const QString& name, QObject* = nullptr);
    Action(const QString& name, QObject* = nullptr);

    static Action* create(const QIcon& icon, const QString& name, const QString& tip, QKeySequence seq = QKeySequence());
    static Action* create(const QString& name, const QString& tip, QKeySequence seq = QKeySequence());

};

