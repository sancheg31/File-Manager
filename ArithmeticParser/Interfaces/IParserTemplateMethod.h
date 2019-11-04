#pragma once

#include <QString>
#include <QVariant>

class IParserTemplateMethod
{
public:
    virtual ~IParserTemplateMethod();
    virtual QVariant parse(const QString&) const;

protected:
    virtual QString removeSpaces(const QString&) const = 0;
    virtual QVariant evalExpression(const QString &, int &) const = 0;
    virtual QVariant evalTerm(const QString &str, int &, int) const = 0;
    virtual QVariant evalFactor(const QString &str, int &) const = 0;
};
