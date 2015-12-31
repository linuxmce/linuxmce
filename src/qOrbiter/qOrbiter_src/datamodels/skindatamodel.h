#ifndef SKINDATAMODEL_H
#define SKINDATAMODEL_H

#include "skinloader.h"
#include <QAbstractListModel>
#include <datamodels/skindataitem.h>
#include <QList>
#include <QVariant>
#include <QUrl>
#include <qorbitermanager.h>

class SkinLoader;
class SkinDataItem;
class qorbiterManager;


class SkinDataModel : public QAbstractListModel
{
    Q_OBJECT
public:

    explicit SkinDataModel(QUrl &baseUrl, SkinDataItem *prototype, qorbiterManager *uiRef, QObject* parent = 0);
    ~SkinDataModel();
#ifdef QT5
   QHash<int, QByteArray> roleNames() const;
#endif
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(SkinDataItem* item);
    void appendRows(const QList<SkinDataItem*> &items);
    void insertRow(int row, SkinDataItem* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    SkinDataItem* takeRow(int row);
    SkinDataItem* find(const QString &id) const;
    QModelIndex indexFromItem( const SkinDataItem* item) const;
    void sortModel(int column, Qt::SortOrder order);
    void clear();
    void addSkin(QString url);
    int *default_ea;
    bool ready;

#ifndef QT5
    QDeclarativeComponent *current_style;
#else
  QQmlComponent * current_style;
#endif
    QObject *currentItem;
    QUrl m_baseUrl;
    QUrl m_entryUrl;
    QUrl m_styleUrl;
public slots:
    void setActiveSkin(QString name);
    void checkStatus();
    void setBaseUrl(QString url){ m_baseUrl=QUrl(url) ;emit baseUrlChanged(); }
private slots:
    void handleItemChange();

signals:
    void skinsFinished(bool b);
    void currentSkinReady();
void baseUrlChanged();

private:
    SkinLoader *m_skin_loader;
    qorbiterManager  *ui_reference;
    SkinDataItem* m_prototype;
    QList<SkinDataItem*> m_list;

};

#endif // SKINDATAMODEL_H
