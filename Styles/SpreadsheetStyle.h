#pragma once

#include <QCommonStyle>


class SpreadsheetStyle : public QCommonStyle {
    Q_OBJECT
public:
    explicit SpreadsheetStyle();

    void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget *w) const;
    void drawItemText(QPainter *painter, const QRect &rect, int flags,
                      const QPalette &pal, bool enabled, const QString &text, QPalette::ColorRole textRole) const;
    void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w) const;

signals:
};
