#ifndef LIGHTINGSCENARIOMODEL_H
#define LIGHTINGSCENARIOMODEL_H


#include <QAbstractListModel>
#include <datamodels/lightingscenarioitem.h>
#include <QList>
#include <QVariant>


  class LightingScenarioItem;


class LightingScenarioModel : public QAbstractListModel
{
    Q_OBJECT
public:

    explicit LightingScenarioModel(LightingScenarioItem *prototype, QObject* parent = 0);
    ~LightingScenarioModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(LightingScenarioItem* item);
    void appendRows(const QList<LightingScenarioItem*> &items);
    void insertRow(int row, LightingScenarioItem* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    LightingScenarioItem* takeRow(int row);
    LightingScenarioItem* find(const QString &id) const;
    QModelIndex indexFromItem( const LightingScenarioItem* item) const;
    void clear();

    int *default_ea;

  private slots:
    void handleItemChange();

  private:
    LightingScenarioItem* m_prototype;
    QList<LightingScenarioItem*> m_list;

};

#endif // LIGHTINGSCENARIOMODEL_H
