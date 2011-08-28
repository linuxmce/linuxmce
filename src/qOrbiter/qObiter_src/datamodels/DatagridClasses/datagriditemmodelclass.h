#ifndef DATAGRIDITEMMODELCLASS_H
#define DATAGRIDITEMMODELCLASS_H

/*
  This class is to be the main handler for the media related datagrids. It is being created from the abstract subclass
  QAbstractItemModel because this is the best abstract class of the various QAbstractModel classes, namely for its item based
  approach which is more important for encapsulating individual file references to be stand alone objects if needed.
  This class will eventually require an imageprovider.
  */

#include <QAbstractItemModel>
#include <QModelIndex>

class DataGridItemModelClass : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit DataGridItemModelClass(QObject *parent = 0);

signals:

public slots:

};

#endif // DATAGRIDITEMMODELCLASS_H
