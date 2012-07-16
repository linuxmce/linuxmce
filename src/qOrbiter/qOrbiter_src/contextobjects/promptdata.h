#ifndef PROMPTDATA_H
#define PROMPTDATA_H

#include <QObject>

class PromptData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString dataTitle READ getTitle WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(int data_id READ getID WRITE setID NOTIFY idChanged)
public:
    explicit PromptData(QString title, int data, QObject *parent = 0);

    QString dataTitle;
    int data_id;
    
signals:
    void titleChanged();
    void idChanged();
    
public slots:
    void setTitle(QString t) { dataTitle = t; emit titleChanged();}
    QString getTitle() {return dataTitle;}

    void setID(int i){ data_id = i; emit idChanged();}
    int getID(){return data_id;}
    
};

#endif // PROMPTDATA_H
