#pragma once

#include <QSettings>

class ISettings {
public:
    virtual QSettings* restoreState() const = 0;
    virtual void saveState(QSettings*) const = 0;
    virtual ~ISettings();
};
