#ifndef ATTRIBUTEMODEL_H
#define ATTRIBUTEMODEL_H

#include <QAbstractItemModel>

class AttributeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit AttributeModel(QObject *parent = 0);
        void sortModel(int column, Qt::SortOrder order);
signals:

public slots:

};

#endif // ATTRIBUTEMODEL_H
