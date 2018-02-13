#ifndef HUETEMPERATURESENSOR_H
#define HUETEMPERATURESENSOR_H

#include <QObject>

class HueTemperatureSensor : public QObject
{
    Q_OBJECT
public:
    explicit HueTemperatureSensor(QObject *parent = 0);

    int getMi_temp() const;
    void setMi_temp(int value);

signals:

public slots:

private:
    int mi_temp;
};

#endif // HUETEMPERATURESENSOR_H
