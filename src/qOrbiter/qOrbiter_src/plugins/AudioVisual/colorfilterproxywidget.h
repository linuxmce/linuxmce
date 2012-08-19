#ifndef COLORFILTERPROXYWIDGET_H
#define COLORFILTERPROXYWIDGET_H

#include <QGraphicsProxyWidget>
#include <QPainter>

class ColorFilterProxyWidget : public QGraphicsProxyWidget
{
    Q_OBJECT

public:
    ColorFilterProxyWidget(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0):
        QGraphicsProxyWidget(parent,wFlags)
    {
    }

signals:
    
public slots:

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
        QPainter *painter_inverted = new QPainter();
        QImage invertedColor(boundingRect().width(),boundingRect().height(),QImage::Format_RGB32);
        painter_inverted->begin(&invertedColor);
        QGraphicsProxyWidget::paint(painter_inverted,option,widget);
        painter_inverted->end();
        painter->drawImage(0,0,invertedColor.rgbSwapped());
    }

    
};

#endif // COLORFILTERPROXYWIDGET_H
