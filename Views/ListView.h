#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QListView>

#include "DefaultViewSettings.h"

class ListView: public QListView, public DefaultViewSettings {
public:
    ListView(QWidget* = nullptr);
};

#endif // LISTVIEW_H
