#pragma once

#include <QAbstractItemView>

class DefaultViewSettings {
protected:
    virtual QAbstractItemView* setDefaultViewSettings(QAbstractItemView*) const;
};
