import QtQuick 2.3
import org.linuxmce.settings 1.0
import "../"
Item{
    id:settingsRow
    height: Style.scaleY(15)
    width: parent.width
    property int cat:SettingsType.Settings_Network
    property int val:SettingsKey.Setting_Network_Router
    property string settingName:""
    property variant settingValue:settings.getNumOption( cat, val)
    property bool useSwitch:false
    signal activated();
    StyledText{
        anchors{
            left:parent.left
            verticalCenter: parent.verticalCenter
            
        }
        text:settingName
    }
    
    StyledText{
        visible: !useSwitch
        anchors{
            right:parent.right
            verticalCenter: parent.verticalCenter
            
        }
        text:settingValue
    }

    Switch {
        id: sw_root
        visible:useSwitch
        enabled: useSwitch ? settingValue : false
        anchors{
            right:parent.right
            verticalCenter: parent.verticalCenter
        }
        onTriggered: {
            activated()
        }
    }
}
