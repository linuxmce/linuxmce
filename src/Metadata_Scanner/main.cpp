#include <QCoreApplication>
#include "scanmanager.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ScanManager manager;

    return a.exec();
}
