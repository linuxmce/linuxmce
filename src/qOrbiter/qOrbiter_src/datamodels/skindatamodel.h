#ifndef SKINDATAMODEL_H
#define SKINDATAMODEL_H

#include "skinloader.h"
#include <QAbstractListModel>
#include <datamodels/skindataitem.h>
#include <QList>
#include <QVariant>
#include <QDeclarativeView>
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
    void clear();
    void addSkin(QString url);
    int *default_ea;

public slots:
    void setActiveSkin(QString name);

  private slots:
    void handleItemChange();

  private:
    SkinLoader *m_skin_loader;
    QUrl m_baseUrl;
    qorbiterManager  *ui_reference;
    SkinDataItem* m_prototype;
    QList<SkinDataItem*> m_list;
};

#endif // SKINDATAMODEL_H
