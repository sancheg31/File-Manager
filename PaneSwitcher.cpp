
#include "PaneSwitcher.h"


PaneSwitcher::PaneSwitcher(Pane* l, Pane* r, QObject* obj): QObject(obj), lPane(l), rPane(r), actPane(nullptr) {

}

Pane* PaneSwitcher::getPane(PaneType type) const {

    switch(type) {
    case PaneType::LeftPane:
        return lPane;
    case PaneType::RightPane:
        return rPane;
    case PaneType::ActivePane:
        return actPane;
    }
    return nullptr;
}

bool PaneSwitcher::isActive(Pane* p) const {
    return (actPane == p) ? true : false;
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
    actPane->setViewTo(Pane::ViewMode::TreeViewMode);
}

void PaneSwitcher::slotToggleToIconView() {
    actPane->setViewTo(Pane::ViewMode::ListViewMode);
}

void PaneSwitcher::slotToggleToTileView() {
    actPane->setViewTo(Pane::ViewMode::TableViewMode);
}


