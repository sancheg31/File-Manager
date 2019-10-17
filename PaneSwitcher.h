#pragma once

#include <QWidget>

#include "Pane.h"

class PaneSwitcher: public QObject {
    Q_OBJECT
public:
    PaneSwitcher(Pane*, Pane*, QObject* = nullptr);

    Pane* activePane() const;
    Pane* inactivePane() const;
    Pane* leftPane() const;
    Pane* rightPane() const;
    bool isActive(Pane*) const;

    void setActivePane(Pane*);
    void switchActivePane();

private slots:
    void slotToggleToDetailView();
    void slotToggleToIconView();
    void slotToggleToTileView();

private:
    Pane* lPane;
    Pane* rPane;
    Pane* actPane;
};
