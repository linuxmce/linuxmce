#ifndef HUEBRIDGE_H
#define HUEBRIDGE_H

#include <QObject>

class HueBridge : public QObject
{
Q_OBJECT
public:
    explicit HueBridge(QObject *parent = 0);

signals:

public slots:

};

#endif // HUEBRIDGE_H
