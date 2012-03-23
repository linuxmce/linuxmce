#include "skinloader.h"
#include "qorbitermanager.h"


SkinLoader::SkinLoader(QUrl baseUrl, qorbiterManager *uiRef, SkinDataModel *parent): ui_reference(uiRef), m_base_url(baseUrl), m_parent(parent)
{

}


SkinLoader::~SkinLoader() {}

void SkinLoader::loadSkin(QString name) {
    // qDebug() << "skinloader adding::" << name;
    //skinBase.setPath(m_baseUrl.toString());
    QUrl style;
    style.setUrl(m_base_url.toString() + "/" + name + "/Style.qml");
    // qDebug() <<style;

    ui_reference->setDceResponse("Skin loader is loading Style.qml: " + name);
    current_component = new QDeclarativeComponent(ui_reference->qorbiterUIwin->engine(), style);
    if (current_component->isLoading()) {
        //qDebug() << "Hooking up slot";

        if (QObject::connect(current_component, SIGNAL(statusChanged(QDeclarativeComponent::Status)),this, SLOT(checkLoadingStatus())))
        {
            //qDebug() << "Hooked!";
        } else {
            //qDebug() << "Fooked! :-(";
        }
    }
    else {
        //qDebug() << "Loaded already?  Attempting to ->create()";
        continueLoading();
    }



    //return new SkinDataItem(QUrl(""), QString("h"), QString("h"), QString("h"), QString("h"), QString("h"), QImage(), QString("h"), QString("h"), QString("h"));
}

void SkinLoader::continueLoading() {
    ui_reference->setDceResponse("Loading will continue now ..");
    QImage skinPic(":/icons/playlist.png");


    if (current_component->isError() || current_component->isLoading()) {
        qWarning() << current_component->errors();
        exit (1);
    } else {
        //qDebug() << "Making style object";
        styleObject = current_component->create(ui_reference->qorbiterUIwin->rootContext());
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

        m_parent->appendRow(new SkinDataItem(skinBase, s_title, s_creator, s_description, s_version, s_target, skinPic, s_path, s_mainc, s_accentc, styleObject));
    }
}

void SkinLoader::checkLoadingStatus()
{

    if(current_component->status() == QDeclarativeComponent::Ready)
        continueLoading();
}
