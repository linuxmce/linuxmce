#include "skinloader.h"
#include "qorbitermanager.h"
#ifndef QT5
#include <QDeclarativeEngine>
#include  <QDeclarativeContext>
#else
#include <QtQml/QQmlEngine>

#endif
SkinLoader::SkinLoader(QUrl baseUrl, qorbiterManager *uiRef, SkinDataModel *parent): ui_reference(uiRef), m_base_url(baseUrl), m_parent(parent)
{
    totalSkinsToLoad = 0;
    loadercounter= 0;

#ifdef QT4_8
    current_component = new QDeclarativeComponent(ui_reference->qorbiterUIwin->rootContext()->engine(),this);
#elif QT5
    current_component = new QQmlComponent(ui_reference->qorbiterUIwin->rootContext()->engine(),this);
#endif
    qDebug("Skin loader constructor complete");
}


SkinLoader::~SkinLoader() {}

void SkinLoader::loadSkin(QString name) {
    qDebug() << "Processed Items Counter::" << loadercounter;
    qDebug() << "skins to load::" << skinsToLoad.size();
    QUrl style;
    style.setUrl(m_base_url.toString() + "/" + name + "/Style.qml");
    qDebug() <<"Processing Style::"<<style;

    ui_reference->setDceResponse("Skin loader is loading Style.qml: " + name);
#if (QT5)
    current_component = new QQmlComponent(ui_reference->qorbiterUIwin->engine(), style);
#else
    current_component = new QDeclarativeComponent(ui_reference->qorbiterUIwin->engine(), style);

#endif
    current_component->loadUrl(style); //= new QDeclarativeComponent(ui_reference->qorbiterUIwin->engine(), style);
    if (current_component->isLoading()) {
        //qDebug() << "Hooking up slot";
#ifdef QT4_8
        if (QObject::connect(current_component, SIGNAL(statusChanged(QDeclarativeComponent::Status)),this, SLOT(continueLoading())))
#elif QT5
        if (QObject::connect(current_component, SIGNAL(statusChanged(QQmlComponent::Status)),this, SLOT(continueLoading())))
#endif
        {
            //qDebug() << "Hooked!";
        } else {
            //qDebug() << "Fooked! :-(";
        }
    }
    else if(current_component->isError())
    {

        qWarning() << current_component->errors();
        qWarning() << "Style.qml is corrupt for skin at index "<< loadercounter << " of " << totalSkinsToLoad << " , moving to next skin. \n";

        if(!skinsToLoad.isEmpty()){
            qWarning() << "Attempting to remove damaged Style.qml from list of availible skins";
            skinsToLoad.takeAt(loadercounter);
            totalSkinsToLoad = skinsToLoad.size();
            qWarning() << "Updated the list of skins to " << totalSkinsToLoad;
        }
        else{

            qWarning() << "No Other Availible skins, setting error screen.";
            exit(99);
        }

        if(loadercounter != totalSkinsToLoad){

            loadSkin(skinsToLoad.at(loadercounter-1));
        }
        else{
            emit finishedList();
        }
    }
    else {
        //qDebug() << "Loaded already?  Attempting to ->create()";
        continueLoading();
    }
}

void SkinLoader::continueLoading() {
    ui_reference->setDceResponse("Loading will continue now ..");
    QImage skinPic(":/icons/playlist.png");

    if (current_component->isError() || current_component->isLoading()) {
        qWarning() << current_component->errors();
        loadSkin(skinsToLoad.at(loadercounter));
    } else {

        //qDebug() << "Making style object";
        styleObject = current_component->create(ui_reference->qorbiterUIwin->rootContext());
        styleObject->setObjectName("temp_data");
        QString s_title = styleObject->property("skinname").toString();
        QString s_creator = styleObject->property("skincreator").toString();
        QString s_description = styleObject->property("skindescription").toString();
        QString s_version = styleObject->property("skinversion").toString();
        QString s_target = styleObject->property("skinvariation").toString();
        QString s_path = styleObject->property("skindir").toString();
        QString s_mainc = styleObject->property("maincolor").toString();
        QString s_accentc = styleObject->property("accentcolor").toString();
        QUrl skinBase(m_base_url.toString()+"/"+s_path);
        ui_reference->setDceResponse("Adding skin to list" + s_title);
        m_parent->appendRow(new SkinDataItem(skinBase, s_title, s_creator, s_description, s_version, s_target, skinPic, s_path, s_mainc, s_accentc, current_component));
        loadercounter++;
        delete styleObject;
        qDebug() << "Loading counter: " << loadercounter;

        if (loadercounter == totalSkinsToLoad)
        {
            emit finishedList();
        }
        else
        {
            loadSkin(skinsToLoad.at(loadercounter));
        }
    }
}

void SkinLoader::checkLoadingStatus()
{
#if (QT5)
    if(current_component->status() == QQmlComponent::Ready)
#else
    if(current_component->status() == QDeclarativeComponent::Ready)
#endif
        continueLoading();
}

void SkinLoader::prepSkinsToLoad(QString skinlist)
{
    loadercounter = 0;
#ifndef QT5
    skinlist.remove("lib, qt5, splash");
#else
     skinlist.remove("lib, splash");
#endif
    skinsToLoad = skinlist.split(",");
    totalSkinsToLoad = skinsToLoad.size();
    loadSkin(skinsToLoad.first());
}
