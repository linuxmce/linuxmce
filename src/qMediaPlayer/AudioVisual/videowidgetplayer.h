#ifndef VIDEOWIDGETPLAYER_H
#define VIDEOWIDGETPLAYER_H

#include <QWidget>
#include <phonon/Phonon>
class videoWidgetPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit videoWidgetPlayer(QWidget *parent = 0);

    Phonon::VideoPlayer *player;
signals:

public slots:

};

#endif // VIDEOWIDGETPLAYER_H
