import QtQuick 2.2
import org.linuxmce.settings 1.0
import "../"
Item{
    id:settingsRow
    height: Style.scaleY(15)
    width: parent.width
    property int cat:SettingsType.Settings_Network
    property int val:SettingsKey.Setting_Network_Router
    property string settingName:""
    property variant settingValue:settings.getOption( cat, val)
    property bool useSwitch:false
    signal activated();
    StyledText{
        anchors{
            left:parent.left

            verticalCenter: parent.verticalCenter
        }
        fontSize: Style.appFontSize_title
        text:settingName
    }
    
    StyledText{
        visible: !useSwitch
        fontSize: Style.appFontSize_title
        anchors{
            right:parent.right
            verticalCenter: parent.verticalCenter
            
        }
        text:useSwitch ? "" : settingValue
    }

    Switch {
        id: sw_root
        visible:useSwitch
        switchLabel: settingName
        enabled: settingValue

        anchors{
            right:parent.right
            verticalCenter: parent.verticalCenter
        }
        onTriggered: {
            activated()
            settings.setOption(cat,val,!settingValue)
            settingValue = settings.getOption( cat, val)
        }
    }

    MouseArea{
        anchors.fill: parent
        onClicked: activated()
        enabled: !useSwitch
    }
}
