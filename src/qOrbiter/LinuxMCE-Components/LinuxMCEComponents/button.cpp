#include "button.h"

#include <qdeclarative.h>
#include <QPainter>
#include <QRectF>

Button::Button(QDeclarativeItem *parent):
    QDeclarativeItem(parent)
{
    // By default, QDeclarativeItem does not draw anything. If you subclass
    // QDeclarativeItem to create a visual item, you will need to uncomment the
    // following line:
    
    setFlag(ItemHasNoContents, false);


}

Button::~Button()
{

}


void Button::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    QRect btn;
    btn.setSize(QSize(height,width));
    QColor m_color(0, 0, 255, 127);
    QPen pen(m_color, 2);
    painter->setPen(pen);
    painter->setRenderHints(QPainter::Antialiasing, true);
    painter->drawRect(btn); //->drawPie(boundingRect(), 90 * 16, 290 * 16);
}



