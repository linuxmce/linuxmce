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

/*!
 * \brief The DceMediaHelper class is designed to contain subset of the dce interface into one class.
 *
 */
class DceMediaHelper : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief DceMediaHelper. Please note that in the ctor, the parent is qorbiter manager, but is passed in as a qobject
     * \param qorbiter_ptr
     * \param parent
     */
    explicit DceMediaHelper(QObject * qorbiter_ptr,QObject *parent = 0);

signals:
    /*!
     * \brief forwardDceCommand. Connected in qOrbiterManager to forward signals to the dce thread.
     * \param cmd
     * \param cbno
     * \param params
     * \param pResp
     */
    void forwardDceCommand(DCE::PreformedCommand cmd, int cbno = 0, QList<int> params = QList<int>(), PreformedCommandResponse* pResp = NULL);


public slots:
    /*!
     * \brief moveFile. Moves a file (using the F! format) to a given path
     * \param filename
     * \param pathTo
     */
    void moveFile(QString filename, QString pathTo);

    /*!
     * \brief moveMediaStream. Move an active media stream from one entertain area to another.
     * \param eaList
     * \param streamId
     */
    void moveMediaStream(QString eaList, int streamId);


private:
      qorbiterManager *m_manager; /*!< Pointer to qorbiter manager to obtain variables needed for sending commands. */
};

}

#endif // DCEMEDIAHELPER_H
