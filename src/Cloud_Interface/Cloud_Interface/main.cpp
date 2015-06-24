#include <QCoreApplication>
#include <cloudinterfacecotroller.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CloudInterfaceController controller;
    return a.exec();
}
