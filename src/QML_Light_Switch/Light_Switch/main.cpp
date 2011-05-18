#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"
#include "../QML_Light_Switch.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);


   // todo - construct! DCE::QML_Light_Switch *dceSwitch = new DCE::QML_Light_Switch();


    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/Light_Switch/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
