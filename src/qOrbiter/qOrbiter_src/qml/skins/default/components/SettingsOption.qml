import QtQuick 2.2
import org.linuxmce.settings 1.0
import QtQuick.Controls 1.2 as QtControls
import "../"
Item{
    id:settingsRow
    height: appStyle.scaleY(15)
    width: parent.width
    property int cat:SettingsType.Settings_Network
    property int val:SettingsKey.Setting_Network_Router
    property string settingName:""
    property variant settingValue:settings.getOption( cat, val)
    property bool useSwitch:false
    property bool useSpinbox:false
    property bool useTextInput:false

    signal valueChanged(int localValue)
    signal textValueChanged(string text)
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

    onUseTextInputChanged: {
        if(useTextInput) control_component.sourceComponent=text_input
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
            minimumValue: -10
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

    Component{
        id:text_input

        Rectangle{
            anchors{
                right:parent.right
                verticalCenter: parent.verticalCenter
            }
            width: appStyle.scaleX(25)
            height: appStyle.scaleY(5)
            color:"darkgrey"
            border.color: "blue"
            border.width: 2
            TextInput{
            anchors.fill: parent
                Component.onCompleted: {
                    ms.enabled=false
                }

                text: String(settingValue)
                font.pixelSize:  appStyle.appFontSize_list
                color: "white"
                onAccepted: {
                    settingsRow.valueChanged(text)
                    settings.setOption(cat,val,text)
                    settingValue = settings.getOption( cat, val)
                }
            }
        }


    }

    StyledText{
        id:label
        anchors{
            left:parent.left

            verticalCenter: parent.verticalCenter
        }
        fontSize: appStyle.appFontSize_list
        text:settingName
    }
    
    StyledText{
        id:settingValDisplay
        visible: !useSwitch
        fontSize: appStyle.appFontSize_list
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
        id:ms
        anchors.fill: parent
        onClicked: activated()
        enabled: !useSwitch && !useSpinbox
    }
}
