#include <QtGui/QApplication>
#include <switchmanager.h>
#include "qmlapplicationviewer.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

   switchManager *dceLightSwitch = new switchManager;
   dceLightSwitch->show();

    return app.exec();
}
