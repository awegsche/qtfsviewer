#ifndef DARKSTYLE_H
#define DARKSTYLE_H

#include <QProxyStyle>

class DarkStyle : public QProxyStyle
{
    Q_OBJECT
public:
    DarkStyle();

    const static int RADIUS = 4;

    // QStyle interface
public:
    //void polish(QWidget *widget);
    //void unpolish(QWidget *widget);

    // QStyle interface

private:
    //mutable QPalette m_standardPalette;

    // QStyle interface
public:
    //void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w) const;
    //static QPainterPath roundRectPath(const QRect& rect);
};

#endif // DARKSTYLE_H
