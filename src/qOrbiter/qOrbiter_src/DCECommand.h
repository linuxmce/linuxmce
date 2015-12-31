#ifndef DCECOMMAND_H
#define DCECOMMAND_H

#include <DCE/PreformedCommand.h>

namespace DCE
{

class DCECommand {
    int m_iCallback;
    PreformedCommand *m_pCmd;
    QHash<int, string*> m_hashParams;

public:
    DCECommand(int callbackno) :
    m_pCmd(NULL), m_iCallback(0)
    {
        m_iCallback = callbackno;
    }

    ~DCECommand()
    {
        if (m_pCmd) {
            delete m_pCmd;
            m_pCmd = NULL;
        }
        for (int i = 0; i < m_hashParams.size(); i++) {
            int key = m_hashParams.keys()[i];
            if (m_hashParams.value(key) != NULL) {
               delete m_hashParams.value(key);
            }
        }
    }

    void setCommand(PreformedCommand *pCmd) {
        m_pCmd = pCmd;
    }

    string* getString(int paramNo) {
        string *pString = new string();
        m_hashParams.insert(paramNo, pString);
        return pString;
    }

    int getCallback() {
        return m_iCallback;
    }

    PreformedCommand *getCommand() {
        return m_pCmd;
    }

    QVariantMap getVariantMap() {
        QVariantMap map;
        for (int i = 0; i < m_hashParams.size(); i++) {
            int key = m_hashParams.keys()[i];
            LoggerWrapper::GetInstance()->Write(LV_STATUS, "DCECommand.getVariantHash() key = %d : %s", key, (*m_hashParams.value(key)).c_str());
            map.insert(QString::number(key), QString::fromStdString(*m_hashParams.value(key)));
        }
        return map;
    }
};

}
#endif // DCECOMMAND_H
