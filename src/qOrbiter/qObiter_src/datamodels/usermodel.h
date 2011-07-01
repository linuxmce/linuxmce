#ifndef USERMODEL_H
#define USERMODEL_H

#include <QAbstractListModel>
#include <datamodels/useritem.h>
#include <QList>
#include <QVariant>

class UserModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit UserModel(UserItem* prototype, QObject* parent = 0);
    ~UserModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(UserItem* item);
    void appendRows(const QList<UserItem*> &items);
    void insertRow(int row, UserItem* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    UserItem* takeRow(int row);
    UserItem* find(const QString &id) const;
    QString  findDefault(int &defaultUser) const;
    QModelIndex indexFromItem( const UserItem* item) const;
    void clear();

    int defaultUser;
  private slots:
    void handleItemChange();

  private:
    UserItem* m_prototype;
    QList<UserItem*> m_list;

};

#endif // USERMODEL_H
