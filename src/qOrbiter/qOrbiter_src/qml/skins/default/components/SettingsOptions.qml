import QtQuick 2.3
import org.linuxmce.settings 1.0
import "../"
Item{
    id:settingsOption
    Flickable{
        anchors.fill: parent
        Column{
            width: parent.width *.85
            height: childrenRect.height
            anchors.horizontalCenter: parent.horizontalCenter
            SettingsOption {
                id: router_addy
                cat:SettingsType.Settings_Network
                val:SettingsKey.Setting_Network_Router
                settingName: qsTr("Router Address")

            }
            SettingsOption {
                id: settingsRow
                cat:SettingsType.Settings_UI
                val:SettingsKey.Setting_Ui_NetworkLoading
                useSwitch: true
                settingName: qsTr("Using Network Skins")

            }
        }


    }
}
