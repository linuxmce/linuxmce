#ifndef VIDEOWIDGETPLAYER_H
#define VIDEOWIDGETPLAYER_H
#ifndef ANDROID
#ifdef QT5
#include <QtWidgets/QWidget>
#else
#include <QWidget>
#include <phonon/Phonon>
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#endif

class ColorFilterProxyWidget : public QGraphicsProxyWidget
{
public:
    ColorFilterProxyWidget(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0):
    QGraphicsProxyWidget(parent,wFlags)
    {
    }

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

class videoWidgetPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit videoWidgetPlayer(QWidget *parent = 0);
    Phonon::MediaObject *qPlayer;
    Phonon::VideoWidget *videoOutput;
    ColorFilterProxyWidget *hack;

signals:

public slots:


};
#endif
#endif // VIDEOWIDGETPLAYER_H
