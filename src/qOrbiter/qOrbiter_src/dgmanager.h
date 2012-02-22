#ifndef DGMANAGER_H
#define DGMANAGER_H

#include <QObject>

class dgManager : public QObject
{
    Q_OBJECT
public:
    explicit dgManager(QObject *parent = 0);

    int mediaType;
    int mediaSubType;
    int fileFormat;
    QString attribte_genres;
    QString mediaSources;
    QString pk_users;
    QString last_viewed;
    int pk_attribute;
    QString users_private;
    QString previousPage;


signals:
    
public slots:
    void prepareRequest();
    void adjustFilters();

    
};

#endif // DGMANAGER_H
