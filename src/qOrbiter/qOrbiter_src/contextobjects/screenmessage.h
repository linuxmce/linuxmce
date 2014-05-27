#ifndef SCREENMESSAGE_H
#define SCREENMESSAGE_H

#include <QObject>

class ScreenMessage : public QObject
{
    Q_OBJECT
public:
      ScreenMessage(QString incMsg, int dur, bool crit);
    explicit ScreenMessage(QObject *parent = 0);

    QString message;
    int duration;
    bool critical;
    
signals:
    
public slots:
    
};

#endif // SCREENMESSAGE_H
