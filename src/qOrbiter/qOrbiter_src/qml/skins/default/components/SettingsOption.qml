import QtQuick 2.2
import org.linuxmce.settings 1.0
import QtQuick.Controls 1.2 as QtControls
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
    property bool useSpinbox:false

    signal valueChanged(int localValue)
    function save(){

    }

    onSettingValueChanged: {
        var t = typeof(settingValue)
        console.log("Setting value type " + t)
    }

    onUseSpinboxChanged: {
         control_component.sourceComponent=combo_bx
    }

    onUseSwitchChanged: {
        if(useSwitch) control_component.sourceComponent=switch_comp
    }

    signal activated();

    Component{
        id:switch_comp
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
    }

    Component{
        id:combo_bx
        QtControls.SpinBox{
            anchors{
                right:parent.right
                verticalCenter: parent.verticalCenter
            }
            value: Number(settingValue)
            onValueChanged:{
                settingsRow.valueChanged(value)
               }
        }
    }

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


    Loader{
        id:control_component
        anchors{
            right:parent.right
            verticalCenter: parent.verticalCenter
        }
    }

    MouseArea{
        anchors.fill: parent
        onClicked: activated()
        enabled: !useSwitch && !useSpinbox
    }
}
