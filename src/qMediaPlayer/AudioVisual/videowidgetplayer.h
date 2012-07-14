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


signals:

public slots:

};
#endif
#endif // VIDEOWIDGETPLAYER_H
