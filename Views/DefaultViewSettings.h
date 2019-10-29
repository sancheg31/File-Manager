#ifndef DEFAULTVIEWSETTINGS_H
#define DEFAULTVIEWSETTINGS_H

#include <QAbstractItemView>

class DefaultViewSettings {
protected:
    virtual QAbstractItemView* setDefaultViewSettings(QAbstractItemView*) const;
};

#endif // DEFAULTVIEWSETTINGS_H
