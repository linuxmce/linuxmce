#ifndef DCEMEDIAHELPER_H
#define DCEMEDIAHELPER_H

#include <QObject>
#include "Gen_Devices/AllCommandsRequests.h"
#include "qhash.h"
#include "qvariant.h"
#include "QVariantMap"
#include "PreformedCommand.h"
#include "DCECommand.h"
#include "DCE/Logger.h"
#include "qdebug.h"
class qorbiterManager;

namespace DCE{


class DceMediaHelper : public QObject
{
    Q_OBJECT
public:
    explicit DceMediaHelper(QObject * qorbiter_ptr,QObject *parent = 0);

signals:
    void forwardDceCommand(DCE::PreformedCommand cmd, int cbno = 0, QList<int> params = QList<int>(), PreformedCommandResponse* pResp = NULL);


public slots:

    void moveFile(QString filename, QString pathTo);


private:
    qorbiterManager *m_manager;
};

}

#endif // DCEMEDIAHELPER_H
