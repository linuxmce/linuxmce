#ifndef PARSERMANAGER_H
#define PARSERMANAGER_H

#include <QObject>
#include "genericdefineparser.h"
class ParserManager : public QObject
{
    Q_OBJECT
public:
    explicit ParserManager(QObject *parent = 0);

signals:
   void parserItemDone(QString name);
   void parserListDone(int fails, int success);

public slots:
    void startItem(int idx);
    void addParserItem(GenericDefineParser *p);
    void itemFinished(int idx, bool status);
    void start();

private:

    QList<GenericDefineParser*> m_list;

};

#endif // PARSERMANAGER_H
