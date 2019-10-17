#include "PaneSwitcher.h"


PaneSwitcher::PaneSwitcher(Pane* l, Pane* r, QObject* obj): QObject(obj), lPane(l), rPane(r) {
    actPane = lPane;
}

Pane* PaneSwitcher::activePane() const {
    return actPane;
}

Pane* PaneSwitcher::inactivePane() const {
    return (actPane == rPane) ? lPane : rPane;
}

bool PaneSwitcher::isActive(Pane* p) const {
    return (actPane == p) ? true : false;
}

Pane* PaneSwitcher::rightPane() const {
    return rPane;
}

Pane* PaneSwitcher::leftPane() const {
    return lPane;
}

void PaneSwitcher::setActivePane(Pane* p) {
    actPane = p;
    rPane->setAlternatingRowColors(isActive(rPane));
    lPane->setAlternatingRowColors(isActive(lPane));
}

void PaneSwitcher::switchActivePane() {
    actPane = (actPane == rPane) ? lPane : rPane;
}

void PaneSwitcher::slotToggleToDetailView() {
    activePane()->setViewTo(Pane::ViewMode::TreeViewMode);
}

void PaneSwitcher::slotToggleToIconView() {
    activePane()->setViewTo(Pane::ViewMode::ListViewMode);
}

void PaneSwitcher::slotToggleToTileView() {
    activePane()->setViewTo(Pane::ViewMode::TableViewMode);
}


