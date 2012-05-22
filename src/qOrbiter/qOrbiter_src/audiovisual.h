#ifndef AUDIOVISUAL_H
#define AUDIOVISUAL_H
//this class represents the 'top level' of the QDeclarativeItem. Here is where we take our subclassed video widget and
//place it in a QDeclarativeItem

#include <QObject>

class AudioVisual : public QObject
{
    Q_OBJECT
public:
    explicit AudioVisual(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // AUDIOVISUAL_H
