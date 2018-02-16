#ifndef HUEDAYNIGHTSENSOR_H
#define HUEDAYNIGHTSENSOR_H

#include <QObject>

class HueDayNightSensor : public QObject
{
    Q_OBJECT
public:
    explicit HueDayNightSensor(QObject *parent = 0);

signals:

public slots:
};

#endif // HUEDAYNIGHTSENSOR_H