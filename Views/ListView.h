#pragma once

#include <QListView>

#include "Views/DefaultViewSettings.h"

class ListView: public QListView, public DefaultViewSettings {
public:
    ListView(QWidget* = nullptr);
};
