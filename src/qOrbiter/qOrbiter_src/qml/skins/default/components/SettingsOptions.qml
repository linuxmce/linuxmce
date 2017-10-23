import QtQuick 2.2
import org.linuxmce.settings 1.0
import org.linuxmce.enums 1.0
import org.linuxmce.fonts 1.0

import "../"
Item{
    id:settingsOption
    state:"basic"



    Component{
        id:fontsView
        ListView{
            id:fontView
            clip:true
            model:fontsHelper.fontList
            anchors.fill: parent
            delegate: Item{
                height: appStyle.listViewItemHeight
                width: parent.width
                Rectangle{
                    anchors.fill: parent
                    color:"transparent"
                    border.color: "white"
                    border.width: 2
                }

                StyledText{
                    anchors.centerIn: parent
                    color: "white"
                    text:model.modelData.familyName
                    font.family: model.modelData.familyName
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: fontsHelper.currentFont = model.modelData.familyName
                }
            }
        }
    }

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
                val:SettingsKey.Setting_Network_Last_Used
                settingName: qsTr("Current Router")
            }
            SettingsOption {
                id: home_router
                cat:SettingsType.Settings_Network
                val:SettingsKey.Setting_Network_Router
                settingName: qsTr("Internal Address")
                useTextInput: true
            }
            SettingsOption {
                id: ext_router
                cat:SettingsType.Settings_Network
                val:SettingsKey.Setting_Network_ExternalHostname
                settingName: qsTr("External Address")
                useTextInput: true
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
                settingName: qsTr("Font Options")
                onActivated: {
                    viewHolder.sourceComponent = fontsView
                    settingsOption.state="extended"
                }
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

    Item{
        id:extendedOptions
        anchors.fill: parent
        Loader{
            id:viewHolder
            anchors.fill: parent
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
