#ifndef MODELPAGE_H
#define MODELPAGE_H

#include <QObject>

class modelPage : public QObject
{
    Q_OBJECT
    Q_PROPERTY (int internalIndex READ getIndex WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY (QString label READ getLabel WRITE setLabel NOTIFY labelChanged)

public:
    explicit modelPage( int internalIndex, QString label, QObject *parent = 0);

    int internalIndex;
    QString label;

signals:
    void indexChanged();
    void labelChanged();
    
public slots:

    void setIndex(int i) {internalIndex = i; emit indexChanged();}
    int getIndex() { return internalIndex;}

    void setLabel (QString l) {label = l; emit labelChanged();}
    QString getLabel () {return label;}
    
};

#endif // MODELPAGE_H
