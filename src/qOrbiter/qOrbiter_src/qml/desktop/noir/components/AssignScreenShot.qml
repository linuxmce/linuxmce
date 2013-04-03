import QtQuick 1.1


Rectangle {
    id:assignscreenshot
    width: scaleX(65)
    height: scaleY(75)
    radius: 5
    color: "slategrey"
    anchors.centerIn: parent
    Connections{
        target: manager
        onMediaScreenShotReady: screenshotImage.source="image://listprovider/screenshot/"+securityvideo.timestamp

    }

    Text {
        id: labelscreenshot
        text: qsTr("Choose Attribute for screenshot")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
    }

    Component
    {
        id:thumbdelegate
        Rectangle{
            id:attributedelegate
            height: 50
            width: parent.width-1
            color: style.accentcolor
            Text {
                id: propertystring
                text: attributeType
            }

            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: attributedelegate.color = "aliceblue"
                onExited: attributedelegate.color = style.accentcolor
                onClicked: {
                     manager.saveScreenShot(attributeNo)
                    manager.cleanupScreenie()
                     assignscreenshot.destroy()
                }
            }

            /* Text {
            id: propertystring2
            text: attributeNo
        } */
        }
    }

    Rectangle{
        id:attributes
        x: 50
        y:50
        height: parent.height *.85
        width: parent.width *.25
        color: style.lighthighlight
        radius:10
        clip:true
        Text {
            id: label2
            text: qsTr("choose attribute")
        }
        ListView{
            id:propertydisplay
            model:screenshotAttributes
            delegate: thumbdelegate
            anchors.fill: attributes
            height: parent.height
            width: parent.width
            visible: true
        }
    }

    Image {
        id: screenshotImage
        anchors.left: attributes.right
        anchors.leftMargin: 10
        source: ""
        height: parent.height *.65
        fillMode: Image.PreserveAspectFit
        width: parent.width*.50
        onSourceChanged: console.log("image updated")
    }

    ButtonSq
    {
        height: scaleY(5)
        width: scaleX(5)
        buttontext: "Close"
        buttonsqradius: 10
        anchors.top: parent.top
        anchors.topMargin: scaleY(1)
        anchors.right: parent.right
        anchors.rightMargin: scaleX(1)
        MouseArea{
            anchors.fill: parent           
            onClicked: {
                manager.cleanupScreenie()
                assignscreenshot.destroy()
            }
        }
    }

}
