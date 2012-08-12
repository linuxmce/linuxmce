#ifndef VIDEOWIDGETPLAYER_H
#define VIDEOWIDGETPLAYER_H
#ifndef ANDROID
#ifdef QT5
#include <QtWidgets/QWidget>
#else
#include <QWidget>
#include <phonon/Phonon>
#endif

class videoWidgetPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit videoWidgetPlayer(QWidget *parent = 0);
    Phonon::MediaObject *qPlayer;
    Phonon::VideoWidget *videoOutput;

signals:

public slots:

};
#endif
#endif // VIDEOWIDGETPLAYER_H
