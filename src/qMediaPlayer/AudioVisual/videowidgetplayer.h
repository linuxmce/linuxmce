#ifndef VIDEOWIDGETPLAYER_H
#define VIDEOWIDGETPLAYER_H

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


signals:

public slots:

};

#endif // VIDEOWIDGETPLAYER_H
