import QtQuick 1.1
import "../components"

Item{
    id:advancedrow
    property int localButtonH:scaleY(10)

    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
    }

    Rectangle{
        anchors.fill: parent
        color: "green"
        opacity: .85
    }

    VisualItemModel {
        id:buttons

        StyledButton{
            height: localButtonH
            width:advancedrow.width-10
            anchors.horizontalCenter: parent.horizontalCenter
            useBorder: true
            buttonText: qsTr("Play All")
            onActivated:{manager.playMedia("!G"+iPK_Device); showOptions = !showOptions }
        }
        StyledButton{
            height: localButtonH
            width:advancedrow.width-10
            useBorder: true
            anchors.horizontalCenter: parent.horizontalCenter
            buttonText: qsTr("Attribute\nSort")
            onActivated:{filterTarget.currentModel = attribfilter; showOptions = !showOptions }
        }
        StyledButton{
            height: localButtonH
            width:advancedrow.width-10
            anchors.horizontalCenter: parent.horizontalCenter
            useBorder: true
            buttonText: qsTr("Genre")
            onActivated:{filterTarget.currentModel = genrefilter; showOptions = !showOptions }
        }
        StyledButton{
            height: localButtonH
            width:advancedrow.width-10
            anchors.horizontalCenter: parent.horizontalCenter
            useBorder: true
            buttonText: qsTr("Sources")
            onActivated:{filterTarget.currentModel = mediasourcelist; showOptions = !showOptions }
        }
        StyledButton{
            height: localButtonH
            width:advancedrow.width-10
            anchors.horizontalCenter: parent.horizontalCenter
            useBorder: false
            buttonText: qsTr("Resolution")
            onActivated:{filterTarget.currentModel = fileformatmodel; showOptions = !showOptions }
        }
        StyledButton{
            height: localButtonH
            width:advancedrow.width-10
            anchors.horizontalCenter: parent.horizontalCenter
            useBorder: true
            buttonText: qsTr("Mediatypes")
            onActivated:{filterTarget.currentModel = mediatypefilter; showOptions = !showOptions }
        }
        StyledButton{
            height: localButtonH
            width:advancedrow.width-10
            anchors.horizontalCenter: parent.horizontalCenter
            useBorder: true
            buttonText: qsTr("Users")
            onActivated:{filterTarget.currentModel = mediatypefilter; showOptions = !showOptions }
        }

        StyledButton{
            height: localButtonH
            width:advancedrow.width-10
            anchors.horizontalCenter: parent.horizontalCenter
            useBorder: true
            buttonText: qsTr("Home")
            onActivated:{manager.gotoQScreen("Screen_1.qml"); showOptions = !showOptions }
        }
    }

    ListView{
        id:btnDisplay
        anchors.top: parent.top
        model:buttons
        height:advancedrow.height
        width: advancedrow.width
        spacing:10
    }



}
