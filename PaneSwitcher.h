#pragma once

#include <QWidget>

#include "Pane.h"

class PaneSwitcher: public QObject {
    Q_OBJECT
public:

    PaneSwitcher(Pane*, Pane*, QObject* = nullptr);

    enum PaneType {
        LeftPane = 1,
        RightPane = 2,
        ActivePane = 4
    };

    Pane* getPane(PaneType) const;

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
