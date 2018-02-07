#include <QCoreApplication>
#include "metadatamanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MetadataManager metaMangager;

    return a.exec();
}

