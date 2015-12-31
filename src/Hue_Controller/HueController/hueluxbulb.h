#ifndef HUELUXBULB_H
#define HUELUXBULB_H
#include <QObject>
#include <QMap>
#include "abstractwirelessbulb.h"
class QVariant;
class HueLuxBulb : public AbstractWirelessBulb
{
    Q_OBJECT
public:
     explicit HueLuxBulb(HueControllerHardware *hw);
    HueLuxBulb(){}

signals:

public slots:
    void proccessStateInformation(QVariantMap d);
};

#endif // HUELUXBULB_H
