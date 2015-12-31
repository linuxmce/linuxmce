/*
    This file is part of QOrbiter for use with the LinuxMCE project found at http://www.linuxmce.org
    Author: Langston Ball
   Langston Ball  golgoj4@gmail.com
    QOrbiter is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QOrbiter is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QOrbiter.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef COLORFILTERPROXYWIDGET_H
#define COLORFILTERPROXYWIDGET_H

#ifdef QT5
#include <QtWidgets/QGraphicsProxyWidget>

#else
#include <QGraphicsProxyWidget>
#endif
#include <QPainter>

class ColorFilterProxyWidget : public QGraphicsProxyWidget
{
    Q_OBJECT

public:
    ColorFilterProxyWidget(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0):
        QGraphicsProxyWidget(parent,wFlags)
    {
        invert = false;
        enable=true;
    }

    bool invert;
    bool enable;
signals:
    
public slots:

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){

        if(enable){
            QPainter *painter_inverted = new QPainter();

            QImage invertedColor(boundingRect().width(),boundingRect().height(),QImage::Format_RGB32);
            painter_inverted->begin(&invertedColor);
            QGraphicsProxyWidget::paint(painter_inverted,option,widget);
            painter_inverted->end();
            if(invert)
                painter->drawImage(0,0,invertedColor.rgbSwapped());
            else
                painter->drawImage(0,0,invertedColor);

        } else {
            QPainter *reg_painter = new QPainter();

            QImage regColor(boundingRect().width(),boundingRect().height(), QImage::Format_RGB32);

            reg_painter->begin(&regColor);
            QGraphicsProxyWidget::paint(reg_painter,option,widget);
            reg_painter->end();


            painter->drawImage(0,0,regColor, Qt::ColorOnly);
        }

    }


    
};

#endif // COLORFILTERPROXYWIDGET_H
