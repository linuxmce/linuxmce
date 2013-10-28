import QtQuick 1.1

Item {
    width: parent.width
    height: parent.height

    Component.onCompleted: {
        manager.grabFileImage()
    }

    Connections{
        target: manager
        onMediaScreenShotReady: screenshotImage.source="image://listprovider/screenshot/"+securityvideo.timestamp
    }

    Item{
        id:imageContainer
        anchors{
            top:parent.top
            bottom:parent.bottom
            left: propertyList.right
            right:parent.right

            Image {
                id: screenshotImage
                anchors.fill: parent
                source: ""
                height: parent.height *.65
                fillMode: Image.PreserveAspectFit
                width: parent.width*.50
                onSourceChanged: console.log("image updated")
            }
        }
    }

    Rectangle{
        id:propertyList
        width: parent.width*.25
        anchors{
            top:parent.top
            left:parent.left
            bottom:parent.bottom
            leftMargin: scaleX(10)
        }
        color:"green"
        StyledText{
            id:propLabel
            text:"Attribtes"
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
        }

        ListView{
            id:propView
            anchors{
                top:propLabel.bottom
                left:parent.left
                right:parent.right
                bottom:parent.bottom
            }
            model:screenshotAttributes
            delegate: StyledButton{
                buttonText: attributeType
                onActivated: {
                    manager.saveScreenShot(attributeNo)
                    // manager.cleanupScreenie()
                    // assignscreenshot.destroy()
                }
            }
        }
    }

}
