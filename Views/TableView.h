#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>

#include "DefaultViewSettings.h"

class TableView : public QTableView, public DefaultViewSettings {
public:
    TableView(QWidget* = nullptr);
};

#endif // TABLEVIEW_H
