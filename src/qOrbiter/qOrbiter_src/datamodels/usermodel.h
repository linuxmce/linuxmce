#ifndef USERMODEL_H
#define USERMODEL_H

#include <QAbstractListModel>
#include <datamodels/useritem.h>
#include <QList>
#include <QVariant>
#include <QDebug>
/*
  The function of this class is to provide a list of users to the user interface. Ita based on QAsbstractItem
  model as well as a custom class. The way it works is two parts.
  The Model Class is essentially a container class for Items in the model. So, in the instance of users,
  there is a Model (UserModel) which takes items (UserItem) which is where the actual data is stored.
  */
class UserModel : public QAbstractListModel
{
    Q_PROPERTY(int currentPrivateUser READ getCurrentPrivateUser WRITE setCurrentPrivateUser NOTIFY privateUserChanged)
    Q_OBJECT
public:
    explicit UserModel(UserItem* prototype, QObject* parent = 0);
    ~UserModel();

#ifdef QT5
    QHash<int, QByteArray> roleNames() const;
#endif
    int currentPrivateUser;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void appendRow(UserItem* item);
    void appendRows(const QList<UserItem*> &items);
    void insertRow(int row, UserItem* item);
    bool removeRow(int row, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    UserItem* takeRow(int row);
    UserItem* find(const QString &id) const;
    void  findDefault() ;
    QModelIndex indexFromItem( const UserItem* item) const;
    void sortModel(int column, Qt::SortOrder order);
    void clear();

    int defaultUser;           //when properly set, it will contain the default user for this orbiter / location
    QString defaultUzer;

signals:
    void privateUserChanged(int u);
    void verifyPinForMedia(int userToValidate);

public slots:
    Q_INVOKABLE void setSelectionStatus(QString format){
        qDebug() << "Setting selection status" << format;
        emit verifyPinForMedia(format.toInt());
        //   currentPrivateUser = format.toInt();
        //   emit privateUserChanged(5, QString::number(currentPrivateUser));
    }

    void unsetPrivate(){
        currentPrivateUser= -1;    
    }

    void setCurrentPrivateUser(int p){  currentPrivateUser = p; emit privateUserChanged(p);}
    int getCurrentPrivateUser(){return currentPrivateUser;}

private slots:
    void handleItemChange();

private:
    UserItem* m_prototype;
    QList<UserItem*> m_list;

};

#endif // USERMODEL_H
