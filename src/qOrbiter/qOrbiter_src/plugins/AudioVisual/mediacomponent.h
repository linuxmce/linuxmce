#ifndef MEDIACOMPONENT_H
#define MEDIACOMPONENT_H

#include <QDeclarativeItem>

class MediaComponent : public QDeclarativeItem
{
    Q_OBJECT
    Q_DISABLE_COPY(MediaComponent)
    
public:
    MediaComponent(QDeclarativeItem *parent = 0);
    ~MediaComponent();
};

QML_DECLARE_TYPE(MediaComponent)

#endif // MEDIACOMPONENT_H

