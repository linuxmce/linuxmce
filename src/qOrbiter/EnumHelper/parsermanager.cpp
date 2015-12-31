#include "parsermanager.h"
#include "QDebug"
ParserManager::ParserManager(QObject *parent) :
    QObject(parent)
{

}

void ParserManager::startItem(int idx)
{
    if(m_list.length() <= idx){
        GenericDefineParser *p = m_list.at(idx);
       if(p->parserReady()){
           p->startParsing();
       }
    }
}

void ParserManager::addParserItem(GenericDefineParser *p)
{
    p->idx=m_list.count();
    QObject::connect(p, SIGNAL(finishedParsing(int ,bool)), this, SLOT(itemFinished(int,bool)));
    m_list.append(p);
    qDebug() << " added item " << p->name() << "to list.";
}

void ParserManager::itemFinished(int idx, bool status)
{
    qDebug() << "Item " << m_list.at(idx)->name() << " completed with success status::"<<status;
    if(m_list.length()==idx){
        emit parserListDone(0,0);
    } else {

        int nextItem = idx++;
        startItem(nextItem);
    }

}

void ParserManager::start()
{
    qDebug() << "Parser Start ";
    qDebug() << "Manager has " << m_list.length() << " items.";
    if(!m_list.isEmpty()){
        m_list.at(0)->startParsing();
    }
}
