#pragma once


class IPrototype {
public:
    virtual ~IPrototype();
    virtual IPrototype* clone() const = 0;
};
