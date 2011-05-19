#ifndef SWITCHWIDGET_H
#define SWITCHWIDGET_H

#include <QMainWindow>
#include <QtDeclarative>

#include "../QML_Light_Switch.h"
#include "Gen_Devices/QML_Light_SwitchBase.h"

namespace DCE
{
class QML_Light_Switch;
}

class switchWidget : public DCE::QML_Light_Switch
{
    Q_OBJECT

public:
    switchWidget(QWidget* parent = 0); //constructor

signals:

public slots:


};

#endif // SWITCHWIDGET_H
