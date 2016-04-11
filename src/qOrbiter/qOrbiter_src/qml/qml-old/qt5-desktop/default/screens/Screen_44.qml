import QtQuick 2.1
import "../components"
import org.linuxmce.settings 1.0
StyledScreen {
    screen: "Advanced Screen"
    description: "Advanced options"
    focusTarget: advanced_flow

    Flow{
        id:advanced_flow
        focus:true
        property int idx:-1
        property int rowFactor: (width / children[0].width) -1
        onIdxChanged: {
            if(idx < 0){
                idx=0
                return;
            } else if (idx >children.length-1){
                idx=children.length-1
                return;
            }
            console.log(rowFactor)

            console.log("focusing child index "+idx)
            advanced_flow.children[idx].forceActiveFocus()
        }

        onActiveFocusChanged: {
            console.log("Flow recieved focus")
            if(idx===-1){
                advanced_flow.idx=0
            } else {

                console.log("idx set, triggering focus")
                advanced_flow.children[idx].forceActiveFocus()
            }


        }
        Keys.onPressed: {
            switch(event.key){
            case Qt.Key_Escape:
                manager.goBacktoQScreen()
                event.accepted=true
                break;
            case Qt.Key_Left:
                idx--
                break;
            case Qt.Key_Right:
                idx++
                break;
            case Qt.Key_Up:
                idx=idx-rowFactor
                break;
            case Qt.Key_Down:
                idx=idx+rowFactor
                break;
            default:
                console.log("Unhandled key==>"+manager.dumpKey(event.key))
            }
        }

        anchors.centerIn: parent
        anchors.fill: parent
        spacing: 25


        StyledButton{
            id:regenorbiter
            state: "large-fixed"
            buttonText: "Regen\nOrbiter"
            onActivated:  regenOrbiter(manager.iPK_Device)
        }

        StyledButton{
            id:pending
            state: "large-fixed"
            buttonText: "Pending\nTasks"

        }
        StyledButton{
            id:clr_settings
            state: "large-fixed"
            buttonText: "Clear\nSettings"
            onActivated: settings.destroySettingsData()
        }

        StyledButton{
            id:networksettings
            state: "large-fixed"
            buttonText: "Network\nSettings"
        }

        StyledButton{
            id:avwizard
            state: "large-fixed"
            buttonText: "Av\nWizard"
        }

        StyledButton{
            id:regenorbiters
            state: "large-fixed"
            buttonText: "Regen All\nOrbiters"

        }

        StyledButton{
            id:quickreload
            state: "large-fixed"
            buttonText: "Quick\n Reload"
            onActivated: manager.quickReload()

        }

        StyledButton{
            id:changeStyle
            state: "large-fixed"
            buttonText: "Change\nStyles"

        }
        StyledButton{
            id:diagnostic
            state: "large-fixed"
            buttonText: "Diagnostics"
            onActivated: content.state="diag"

        }
        StyledButton{
            id:refresh
            state: "large-fixed"
            buttonText: "Clear\nCache"
            onActivated:{ manager.clearSkinCache();
                qmlroot.forceActiveFocus()
                //                manager.goBacktoQScreen()
            }
        }
        StyledButton{
            id:ping
            state: "large-fixed"
            buttonText: "Ping\nTest"
        }
        StyledButton{
            id:internalExternal
            property bool settingsValue:settings.getOption(SettingsType.Settings_UI,SettingsKey.Setting_Ui_NetworkLoading)
            state:"large-fixed"
            buttonText: settingsValue ? "Use Network Skins" : "Use Local Skins"
            onActivated: {
                settings.setOption( Number(SettingsType.Settings_UI),Number(SettingsKey.Setting_Ui_NetworkLoading), !settingsValue)
                settingsValue = !settingsValue
            }
        }
    }

}
