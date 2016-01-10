#ifndef REMOTECOMMANDS_H
#define REMOTECOMMANDS_H
#include <QObject>

/*!
 * \brief The RemoteCommands class. This class is a convienence class designed to make
 * passing the remote commands to the gui as simple as possible. This is a dirty method
 * of doing this, i understand.
 * never the less, import org.linuxmce.enums
 * use: RemoteCommands.Menu
 *  \badcode
 */
class RemoteCommands : public QObject {
    Q_OBJECT
public:
    RemoteCommands(){}
  virtual ~RemoteCommands() {}

    enum DceRemoteCommands {
        BackClearEntry=363,    /*!< For Keyboard use */
        BackPriorMenu=240,     /*!< For intermenu use */
        EnterGo=190,           /*!< Confirm Button */
        Guide=126,             /*!< Any form of guide the orbiter can show */
        Menu=548,              /*!< Contextual Menu */
        MoveDown=201,          /*!< Movie focus down in context */
        MoveLeft=202,          /*!< Move focus left in context */
        MoveRight=203,         /*!< Move focus right in context */
        MoveUp=200,            /*!< Move focus up in contex */
        TogglePower=194        /*!< Toggle the power of the gui*/
    };
    Q_ENUMS(DceRemoteCommands)
};

class UtilityCommands: public QObject {
    Q_OBJECT
public:
    UtilityCommands(){}
   virtual ~UtilityCommands() {}
    enum DceUtilityCommands{
        Off=193,
        On=192
    };
    Q_ENUMS(DceUtilityCommands)
};


#endif // REMOTECOMMANDS_H
