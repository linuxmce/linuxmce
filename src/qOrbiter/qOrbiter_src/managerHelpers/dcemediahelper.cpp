#include "dcemediahelper.h"
#include <qOrbiter/qOrbiter.h>
#include "qorbitermanager.h"

DceMediaHelper::DceMediaHelper(QObject *qorbiter_ptr, RouterHelper *helper, QObject *parent) : QObject(parent), m_routerHelper(helper)
{
    //qOrbiter*ptr = qobject_cast<qOrbiter*>(qorbiter_ptr);

    Q_UNUSED(qorbiter_ptr);
    m_manager = qobject_cast<qorbiterManager*>(parent);
}

void DceMediaHelper::moveFile(QString filename, QString pathTo)
{
    DCE::CMD_Move_File moveFile( m_manager->iPK_Device, m_routerHelper->mediaPluginId(), filename.toStdString(), pathTo.toStdString());
    emit forwardDceCommand(moveFile);
}

void DceMediaHelper::moveMediaStream(QString eaList, int streamId)
{
    CMD_MH_Move_Media move_media( m_manager->iPK_Device,  m_routerHelper->mediaPluginId(), streamId, eaList.toStdString() );
    emit forwardDceCommand(move_media);
}

