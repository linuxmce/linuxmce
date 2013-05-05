#ifndef HUEBRIDGE_H
#define HUEBRIDGE_H

#include <QObject>

#include  "qjson/parser.h"
#include "qjson/qobjecthelper.h"
#include "qjson/serializer.h"
#include "qnetworkaccessmanager.h"
#include "Hue_Controller/Hue_Controller.h"


class HueBridge : public QObject
{
Q_OBJECT
public:
    explicit HueBridge(DCE::Hue_Controller* dceObject, QObject *parent = 0 );

signals:

public slots:

private:
    DCE::Hue_Controller *mp_dceController;
};

#endif // HUEBRIDGE_H
