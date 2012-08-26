#ifndef SCREENSAVERMODULE_H
#define SCREENSAVERMODULE_H

#include <QObject>
#include <QUrl>
#include <QStringList>
#include <QTimer>


class ScreenSaverModule : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QUrl current_image READ getImage WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY (QUrl transition_image READ getTImage WRITE setTImage NOTIFY transitionChanged)
    Q_PROPERTY (QString testtext READ getTest)
    Q_PROPERTY (int timerInterval READ getInterval WRITE setInterval NOTIFY intervalChanged)
    Q_PROPERTY (int transitionInterval READ getTInterval WRITE setTInterval NOTIFY transitionSpeedChanged)

public:
    explicit ScreenSaverModule(QObject *parent = 0);

    QUrl current_image;
    QUrl transition_image;

    QStringList *imageList;
    QString testtext;
    QString getTest() {return testtext;}


    int timerInterval;
    int getInterval() {return timerInterval;}
    void setInterval (int ms) {timerInterval = ms; emit intervalChanged();}

    int transitionInterval;
    int getTInterval() {return transitionInterval;}
    void setTInterval (int ms) {transitionInterval = ms; emit transitionSpeedChanged();}

    QTimer picChanger;
    QTimer picChanger2;

signals:
    void imageChanged();
    void transitionChanged();
    void loadingError();
    void intervalChanged();
    void transitionSpeedChanged();

public slots:
    void init();
    void nextImage();
    void nextTransition();

    QUrl getTImage( ) {return transition_image;}
    void setTImage(const QUrl &q)  { transition_image = q; emit transitionChanged(); }

    QUrl getImage( ) {return current_image;}
    void setImage(const QUrl &q)  { current_image = q; emit imageChanged(); }

    int getNext();

    void startAlternate();

};

#endif // SCREENSAVERMODULE_H
