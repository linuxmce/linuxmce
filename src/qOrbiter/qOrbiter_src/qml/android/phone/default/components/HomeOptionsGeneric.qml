import QtQuick 1.1
import "../components"

Item{
    id:advancedrow
    property int localButtonH:scaleY(15)
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
            useBorder: false
            buttonText: qsTr("Home")
            onActivated:  {manager.gotoQScreen("Screen_1.qml"); showOptions = !showOptions }
        }

    }

    ListView{
        id:btnDisplay
        model:buttons
        height:(buttons.count+1)*localButtonH
    }

}
