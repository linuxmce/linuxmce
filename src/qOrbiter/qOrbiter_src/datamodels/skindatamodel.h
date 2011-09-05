#ifndef SKINDATAMODEL_H
#define SKINDATAMODEL_H

#include <QAbstractListModel>
#include <datamodels/skindataitem.h>
#include <QList>
#include <QVariant>


  class SkinDataItem;


class SkinDataModel : public QAbstractListModel
{
    Q_OBJECT
public:

    explicit SkinDataModel(SkinDataItem *prototype, QObject* parent = 0);
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

    int *default_ea;

  private slots:
    void handleItemChange();

  private:
    SkinDataItem* m_prototype;
    QList<SkinDataItem*> m_list;

};

#endif // SKINDATAMODEL_H
