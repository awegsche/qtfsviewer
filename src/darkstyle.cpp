#include "darkstyle.h"

#include <QComboBox>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <qdebug.h>

DarkStyle::DarkStyle()
{
    qDebug() << "setting new style";
}
/*

void DarkStyle::polish(QWidget *widget)
{
    if (qobject_cast<QPushButton*>(widget)
            || qobject_cast<QComboBox*>(widget))
        widget->setAttribute(Qt::WA_Hover, true);
}

void DarkStyle::unpolish(QWidget *widget)
{

    if (qobject_cast<QPushButton*>(widget)
            || qobject_cast<QComboBox*>(widget))
        widget->setAttribute(Qt::WA_Hover, false);
}

void DarkStyle::drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    switch (pe) {
    case PE_PanelButtonCommand:
    case PE_Frame:
    {
        int delta = (opt->state & State_MouseOver) ? 64 : 0;
        QColor slightlyOpaqueBlack(0, 0, 0, 63);
        QColor semiTransparentWhite(200, 200, 255, 127 + delta);
        QColor semiTransparentBlack(0, 0, 55, 127 - delta);

        int x, y, width, height;
        opt->rect.getRect(&x, &y, &width, &height);

        QPainterPath roundRect = roundRectPath(opt->rect);
        //int radius = qMin(width, height) / 4;
        int radius = RADIUS;

        QBrush brush;
        bool darker;

        const QStyleOptionButton *buttonOption =
                qstyleoption_cast<const QStyleOptionButton *>(opt);
        if (buttonOption
                && (buttonOption->features & QStyleOptionButton::Flat)) {
            brush = opt->palette.button();
            darker = (opt->state & (State_Sunken | State_On));
        } else {
            if (opt->state & (State_Sunken | State_On)) {
                brush = opt->palette.mid();
                darker = !(opt->state & State_Sunken);
            } else {
                brush = opt->palette.button();
                darker = false;
            }
        }
        p->save();
        p->setRenderHint(QPainter::Antialiasing, true);
        p->fillPath(roundRect, brush);
        if (darker)
            p->fillPath(roundRect, slightlyOpaqueBlack);
        int penWidth;
        penWidth = 1;

        QPen topPen(semiTransparentWhite, penWidth);
        QPen bottomPen(semiTransparentBlack, penWidth);

        if (opt->state & (State_Sunken | State_On))
            qSwap(topPen, bottomPen);
        int x1 = x;
        int x2 = x + radius;
        int x3 = x + width - radius;
        int x4 = x + width;

        if (opt->direction == Qt::RightToLeft) {
            qSwap(x1, x4);
            qSwap(x2, x3);
        }

        QPolygon topHalf;
        topHalf << QPoint(x1, y)
            << QPoint(x4, y)
            << QPoint(x3, y + radius)
            << QPoint(x2, y + height - radius)
            << QPoint(x1, y + height);

        p->setClipPath(roundRect);
        p->setClipRegion(topHalf, Qt::IntersectClip);
        p->setPen(topPen);
        p->drawPath(roundRect);
        QPolygon bottomHalf = topHalf;
        bottomHalf[0] = QPoint(x4, y + height);

        p->setClipPath(roundRect);
        p->setClipRegion(bottomHalf, Qt::IntersectClip);
        p->setPen(bottomPen);
        p->drawPath(roundRect);

        p->setPen(opt->palette.windowText().color());
        p->setClipping(false);
        //p->drawPath(roundRect);

        p->restore();
        break;
    }
    //case PE_PanelItemViewItem:
    //    break;
    default:
        QProxyStyle::drawPrimitive(pe, opt, p, w);
    }
}

QPainterPath DarkStyle::roundRectPath(const QRect &rect)
{
    //int radius = qMin(rect.width(), rect.height()) / 4;
    int radius = RADIUS;
    int diam = 2 * radius;

    int x1, y1, x2, y2;
    rect.getCoords(&x1, &y1, &x2, &y2);

    QPainterPath path;
    path.moveTo(x2, y1 + radius);
    path.arcTo(QRect(x2 - diam, y1, diam, diam), 0.0, +90.0);
    path.lineTo(x1 + radius, y1);
    path.arcTo(QRect(x1, y1, diam, diam), 90.0, +90.0);
    path.lineTo(x1, y2 - radius);
    path.arcTo(QRect(x1, y2 - diam, diam, diam), 180.0, +90.0);
    path.lineTo(x1 + radius, y2);
    path.arcTo(QRect(x2 - diam, y2 - diam, diam, diam), 270.0, +90.0);
    path.closeSubpath();
    return path;
}


*/
