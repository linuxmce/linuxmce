#include "button.h"

#include <qdeclarative.h>
#include <QPainter>
#include <QRectF>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

QtButton::QtButton(QDeclarativeItem *parent):
    QDeclarativeItem(parent)
{
    // By default, QDeclarativeItem does not draw anything. If you subclass
    // QDeclarativeItem to create a visual item, you will need to uncomment the
    // following line:
    
    setFlag(ItemHasNoContents, false);
    setActive(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
    setAcceptTouchEvents(true);
    qDebug() << acceptedMouseButtons();
    qDebug() << acceptTouchEvents();
    borderWidth =0;
    borderColor = Qt::transparent;
    radius = 0;

}

QtButton::~QtButton()
{

}


void QtButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    QRect btn;
    btn.setSize(QSize(width,height));
    QPen pen;

    QBrush brush(color);
    if(borderWidth > 0){
        pen.setColor(borderColor);
        pen.setWidthF(borderWidth);
    }else
    {
        pen.setColor(Qt::transparent);
    }

    if(!m_PaintImage.isNull()){
       // brush.setTextureImage(m_PaintImage.scaledToHeight(height));
    }
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->setRenderHints(QPainter::Antialiasing, true);
    painter->drawRoundRect(btn,radius, radius);

    if(!m_PaintImage.isNull()){
        QRectF container(10.0, 10.0, 50.0, 60.0);
        painter->drawImage(btn, m_PaintImage.scaledToHeight(height), container);
       // brush.setTextureImage(m_PaintImage.scaledToHeight(height));
    }
}

void QtButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setPressed(true);
    emit pressed();
}

void QtButton::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setPressed(false);
    emit released();

}

void QtButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setHovering(true);
    emit hoveringChanged();

}

void QtButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setHovering(false);
    emit hoveringChanged();
}

void QtButton::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    qDebug() << event->scenePos();
}





