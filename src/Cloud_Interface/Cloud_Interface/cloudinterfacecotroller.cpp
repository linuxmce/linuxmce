#include "cloudinterfacecotroller.h"
#include <QPluginLoader>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include <QPluginLoader>
#include <abstractcloudprovider.h>
CloudInterfaceController::CloudInterfaceController(QObject *parent) : QObject(parent)
{
    qDebug() << Q_FUNC_INFO << "starting";
    QDir currentDir(QCoreApplication::applicationDirPath());
    QStringList t= currentDir.entryList();

    QPluginLoader loader;
    for(int i=0; i < t.count(); i++){
        if(t.at(i).endsWith(".so")){
            qDebug () << t.at(i);
            loader.setFileName(t.at(i));
           QObject *obj =loader.instance();
           if(obj){

               CloudServiceInterface *iface =  qobject_cast<CloudServiceInterface*>(obj);
             if(iface){

              m_plugins.insert(iface->name(), iface);
             } else {
                 qDebug() << "!" << loader.errorString();
             }
           } else {
               qDebug() << loader.errorString() << "!";
           }


        }
    }

    qDebug() << m_plugins.count() << " plugins loaded.";
    qDebug() << "Names:\n " << m_plugins.keys();



}

