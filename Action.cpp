#include "Action.h"

Action::Action(QObject * obj): QAction(obj) {

}

Action::Action(const QIcon& icon, const QString& name, QObject * obj): QAction(icon, name, obj) {

}

Action::Action(const QString& name, QObject * obj): QAction(name, obj) {

}

Action* Action::create(const QIcon& icon, const QString& name, const QString& tip, QKeySequence seq) {
    Action* action = new Action(icon, name);
    action->setStatusTip(tip);
    action->setShortcut(seq);
    return action;
}

Action* Action::create(const QString& name, const QString& tip, QKeySequence seq) {
    Action* action = new Action(name);
    action->setStatusTip(tip);
    action->setShortcut(seq);
    return action;
}
