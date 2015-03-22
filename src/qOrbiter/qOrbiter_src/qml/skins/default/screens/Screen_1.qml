import QtQuick 2.3
import "../components"
import "../"
import org.linuxmce.screens 1.0
StyledScreen {
id:screen

    StyledText{
        anchors{
            top:screen.top
            horizontalCenter: parent.horizontalCenter
        }
        color:"white"
        text:"screen 1"
        fontSize: Style.appFontSize_header
    }

    StyledButton{
        buttonText: qsTr("Advanced")
        state:"fixed"
        anchors.centerIn: parent
        onActivated: manager.setCurrentScreen(Screens.AdvancedOptions)
    }

    ListView{
        anchors{
            left:parent.left
            top:parent.top
            bottom:parent.bottom
        }
        model:qmlRoot.scenarios
        width: Style.listViewWidth_medium
        delegate: Item{
            id:listViewLargeDelegate
            height: Style.scaleY(8)
            width: parent.width
            StyledText{
                anchors.centerIn: parent
                text:name
            }
        }
    }

}
