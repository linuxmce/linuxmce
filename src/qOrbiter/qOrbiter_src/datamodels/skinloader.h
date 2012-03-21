#ifndef SKINLOADER_H
#define SKINLOADER_H

#include <QObject>
#include <QUrl>
#include <QList>
#include <QDeclarativeComponent>
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

  private slots:
    void continueLoading();
    void checkLoadingStatus();

  private:
    QDeclarativeComponent *current_component;

    SkinDataModel *m_parent;
    QUrl m_base_url;
    qorbiterManager  *ui_reference;
    QObject *styleObject;
};

#endif // SKINLOADER_H
