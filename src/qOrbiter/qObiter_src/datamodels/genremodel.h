#ifndef GENREMODEL_H
#define GENREMODEL_H

#include <QAbstractItemModel>

class GenreModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit GenreModel(QObject *parent = 0);

signals:

public slots:

};

#endif // GENREMODEL_H
