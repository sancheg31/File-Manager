#pragma once

class ISaveable {
public:
    virtual ~ISaveable();
    virtual void save() const = 0;
    virtual void restore() = 0;

};
