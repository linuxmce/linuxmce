import QtQuick 2.2
import org.linuxmce.settings 1.0
import org.linuxmce.enums 1.0
import "../"
Item{
    id:settingsOption
    state:"basic"
    Flickable{
        id:basicOptions
        anchors.fill: parent
        clip:true
        contentHeight:settings_flickable.height
        Column{
            id:settings_flickable
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
                id: device_id_setting
                cat:SettingsType.Settings_Network
                val:SettingsKey.Setting_Network_Device_ID
                settingName: qsTr("Device")
            }
            SettingsOption {
                id: network_option
                cat:SettingsType.Settings_UI
                val:SettingsKey.Setting_Ui_NetworkLoading
                useSwitch: true
                settingName: qsTr("Using Network Skins")

            }
            SettingsOption {
                id: screensaver_option
                cat:SettingsType.Settings_UI
                val:SettingsKey.Setting_Ui_ScreenSaver
                useSwitch: true
                settingName: qsTr("Enable Screen Saver")

            }
            SettingsOption {
                id: skin_option
                cat:SettingsType.Settings_UI
                val:SettingsKey.Setting_Ui_Skin
                settingName: qsTr("UI Skin")
            }
            SettingsOption {
                id: size_option
                cat:SettingsType.Settings_UI
                val:SettingsKey.Setting_Ui_PrefSize
                settingName: qsTr("Preferred Size")

            }
            SettingsOption {
                id: text_size_option
                cat:SettingsType.Settings_Text
                val:SettingsKey.Setting_Text_sizemod
                settingName: qsTr("Text Modifier:")
                useSpinbox: true
                onValueChanged: {
                    settings.setFontSizeMod(localValue)
                }
            }

            SettingsOption {
                id: font_option
                cat:SettingsType.Settings_Text
                val:SettingsKey.Setting_Text_font
                settingName: qsTr("Font Option")
                onActivated: {extendedView.model=manager.systemFontList();
                    settingsOption.state="extended"}
            }
            SettingsOption {
                id: lang_option
                cat:SettingsType.Settings_Text
                val:SettingsKey.Setting_Text_language
                settingName: qsTr("Language")
            }

            SettingsOption {
                id: media_audioSort_option
                cat:SettingsType.Settings_Media
                val:SettingsKey.Setting_Media_AudioSort
                settingName: qsTr("Default Audio Sorting")
                //settingValue: Attributes.translateType(Number(settings.getOption( cat, val)))
            }
        }
    }

    ListModel{
        id:fonts
        ListElement{
            name:"Times"
        }
        ListElement{
            name:"Times"
        }
        ListElement{
            name:"Times"
        }
        ListElement{
            name:"Times"
        }
        ListElement{
            name:"Times"
        }
    }

    Item{
        id:extendedOptions
        anchors.fill: parent

        ListView{
            id:extendedView
            model:10
            anchors.fill: parent
            delegate: Item{
                height: Style.listViewItemHeight
                width: parent.width
                StyledText{
                    anchors.centerIn: parent
                    color: "white"
                    text:index
                }
            }
        }
    }

    states: [
        State {
            name: "basic"
            PropertyChanges {
                target: basicOptions
                visible:true
                interactive:true
            }
            PropertyChanges{
                target: extendedOptions
                visible:false
            }
        },
        State {
            name: "extended"
            PropertyChanges {
                target: basicOptions
                visible:false
                interactive:false
            }
            PropertyChanges{
                target: extendedOptions
                visible:true
            }
        }
    ]
}
