#ifndef DEFAULTVIEWSETTINGS_H
#define DEFAULTVIEWSETTINGS_H

#include <QAbstractItemView>

class DefaultViewSettings {
protected:
    virtual QAbstractItemView* setDefaultViewSettings(QAbstractItemView*) const;
    virtual ~DefaultViewSettings();
};

#endif // DEFAULTVIEWSETTINGS_H
