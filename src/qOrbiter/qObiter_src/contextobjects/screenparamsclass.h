#ifndef SCREENPARAMSCLASS_H
#define SCREENPARAMSCLASS_H

/*
The purpose of this class is to track screen parameters for certain screens. Params are not always used and/or relevant
to a screen, but in the case they are, there should be proper handling. The purpose of this class is to be an independant
reference to the current screen params and relevant getters and setters.
  */

#include <QObject>
#include <QMap>

class ScreenParamsClass : public QObject
{
    Q_OBJECT
public:
    explicit ScreenParamsClass(QObject *parent = 0);

    int deviceTo;
    int deviceFrom;
    int deviceTemplate;
    int deviceCategory;
    int currentScreen;
    QMap <int, int> paramDevices;

signals:

public slots:


};

#endif // SCREENPARAMSCLASS_H
