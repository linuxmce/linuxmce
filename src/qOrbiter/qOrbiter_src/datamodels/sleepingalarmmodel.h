#ifndef SLEEPINGALARMMODEL_H
#define SLEEPINGALARMMODEL_H

#include <QAbstractListModel>
#include <datamodels/DataModelItems/sleepingalarm.h>

class SleepingAlarmModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SleepingAlarmModel(QObject *parent = 0);
    ~SleepingAlarmModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(SleepingAlarm* item);
    void appendRows(const QList<SleepingAlarm*> &items);
    void insertRow(int row, SleepingAlarm* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    SleepingAlarm* takeRow(int row);
    SleepingAlarm* find(const QString &id) const;
    void sortModel(int column, Qt::SortOrder order);

#ifdef QT5
 QHash<int, QByteArray> roleNames() const;
#endif

 QModelIndex indexFromItem( const SleepingAlarm* item) const;


signals: 
 void modelAboutToBeReset();
 void modelReset();

    
public slots:
void clear();

private slots:
 void handleItemChange();

private:
 bool resetInternalData();
 SleepingAlarm* m_prototype;
 QList<SleepingAlarm*> m_list;
    
};

#endif // SLEEPINGALARMMODEL_H
