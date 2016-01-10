#ifndef SKINLOADER_H
#define SKINLOADER_H

#include <QObject>
#include <QUrl>
#include <QStringList>
#if (QT5)
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlComponent>
#else
#include <QtDeclarative/QDeclarativeComponent>
#include <QDeclarativeItem>
#endif
#include "skindataitem.h"

class SkinDataModel;
class qorbiterManager;

class SkinLoader: public QObject
{
    Q_OBJECT
public:

    explicit SkinLoader(QUrl baseUrl, qorbiterManager *uiRef, SkinDataModel *parent);
    ~SkinLoader();

    void loadSkin(QString name);
    void prepSkinsToLoad(QString skinlist);

  private slots:
    void continueLoading();
    void checkLoadingStatus();

signals:
    void finishedList();
    void skinItemComplete();

  private:
#if (QT5)
    QQmlComponent *current_component;
#else
    QDeclarativeComponent *current_component;
#endif
    SkinDataModel *m_parent;
    QUrl m_base_url;
    qorbiterManager  *ui_reference;
    QObject *styleObject;
    QStringList skinsToLoad;
    int totalSkinsToLoad;
    int loadercounter;
};

#endif // SKINLOADER_H
