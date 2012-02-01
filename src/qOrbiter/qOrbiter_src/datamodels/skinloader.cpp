#include "skinloader.h"
#include "qorbitermanager.h";
#include <QDebug>

SkinLoader::SkinLoader(QUrl baseUrl, qorbiterManager *uiRef, SkinDataModel *parent): ui_reference(uiRef), m_base_url(baseUrl), m_parent(parent)
{

}


SkinLoader::~SkinLoader() {}

void SkinLoader::loadSkin(QString name) {

     //skinBase.setPath(m_baseUrl.toString());
    QUrl style;
    style.setUrl(m_base_url.toString() + "/" + name + "/Style.qml");

    qDebug() << "Skin loader is loading Style.qml: " << name;

    current_component = new QDeclarativeComponent(ui_reference->qorbiterUIwin->engine(), style);
    if (current_component->isLoading()) {
        qDebug() << "Hooking up slot";
//         QObject::connect(current_component, SIGNAL(statusChanged(QDeclarativeComponent::Status)),
//                          this, SLOT(continueLoading()));
        if (QObject::connect(current_component, SIGNAL(statusChanged(QDeclarativeComponent::Status)),
                          this, SLOT(continueLoading()))) {
            qDebug() << "Hooked!";
        } else {
            qDebug() << "Fooked! :-(";
        }
    }
    else {
        qDebug() << "Loaded already?  Attempting to ->create()";
        continueLoading();
    }



    //return new SkinDataItem(QUrl(""), QString("h"), QString("h"), QString("h"), QString("h"), QString("h"), QImage(), QString("h"), QString("h"), QString("h"));
}

void SkinLoader::continueLoading() {
qDebug() << "Loading will continue now ..";
QImage skinPic(":/icons/Skin-Data.png");

QUrl skinBase(m_base_url.toString()  + "/default");
    if (current_component->isError()) {
        qWarning() << current_component->errors();
    } else {
        qDebug() << "Making style object";
        styleObject = current_component->create(ui_reference->qorbiterUIwin->rootContext());
        QString s_title = styleObject->property("skinname").toString();
        QString s_creator = styleObject->property("skincreator").toString();
        QString s_description = styleObject->property("skindescription").toString();
        QString s_version = styleObject->property("skinversion").toString();
        QString s_target = styleObject->property("skinvariation").toString();
        QString s_path = styleObject->property("skindir").toString();
        QString s_mainc = styleObject->property("maincolor").toString();
        QString s_accentc = styleObject->property("accentcolor").toString();
         qDebug() << "Adding skin to list" << s_title;
        m_parent->appendRow(new SkinDataItem(skinBase, s_title, s_creator, s_description, s_version, s_target, skinPic, s_path, s_mainc, s_accentc, styleObject));
    }
}
