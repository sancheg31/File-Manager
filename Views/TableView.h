#pragma once

#include <QTableView>

#include "Views/DefaultViewSettings.h"

class TableView : public QTableView, public DefaultViewSettings {
public:
    TableView(QWidget* = nullptr);
};
