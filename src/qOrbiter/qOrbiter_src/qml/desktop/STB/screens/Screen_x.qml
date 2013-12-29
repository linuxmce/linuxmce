import QtQuick 1.1
import "../components"
StyledScreen {
    Component.onCompleted: failTimeout.start()

    Item{
        width: parent.width *.85
        height: parent.height *.65
        anchors.centerIn: parent

        Rectangle{
            id:hdr
            height: parent.height/3
            gradient: appStyle.alertGradient
            radius:5
            anchors{
                left:parent.left
                right:parent.right
                top:parent.top
            }
        }
        Rectangle{
            id:content

            anchors{
                left:parent.left
                right:parent.right
                top:hdr.bottom
                topMargin: -20
                bottom: parent.bottom
            }
            gradient:appStyle.contentGradient

            StyledText {
                id: name
                text: manager.currentScreen + qsTr(" is  Not Found. Please tell Langston to get it together already.")
                font.pointSize: 32
                font.bold: true
                anchors.centerIn: parent
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                width: content.width
            }
        }

    }

    Timer{
        id:failTimeout
        interval: 5000
        onTriggered: manager.goBacktoQScreen()
    }

}
