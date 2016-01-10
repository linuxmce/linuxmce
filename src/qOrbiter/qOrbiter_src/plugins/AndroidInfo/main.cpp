#include <QApplication>
#include "qmlapplicationviewer.h"
#include <QDeclarativeContext>
#include <androidsystem.h>



Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    QmlApplicationViewer viewer;
    AndroidSystem system;
    viewer.rootContext()->setContextProperty("system", &system);
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/AndroidInfoPlugin/main.qml"));
    viewer.showExpanded();

    return app->exec();
}
