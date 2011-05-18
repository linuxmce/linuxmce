#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"
#include "../QML_Light_Switch.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/Light_Switch/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
