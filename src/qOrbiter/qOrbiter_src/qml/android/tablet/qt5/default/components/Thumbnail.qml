import QtQuick 2.0

Item {
    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
        bottom:parent.bottom
        margins:scaleX(4)
    }

    Component.onCompleted: {
        manager.cleanupScreenie()
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
        }
        Image {
            id: screenshotImage
            anchors.fill: parent
            source: ""
            fillMode: Image.PreserveAspectFit
            onSourceChanged: console.log("image updated")
        }
    }

    HeaderListView{
        id:propertyList
        anchors{
            top:parent.top
            left:parent.left
            bottom:parent.bottom
        }
        width:parent.width*.33
        listTitle:"Attributes"
        displayProperty: "attributeType"
        returnProperties: ["attributeNo"]
        headerListModel:screenshotAttributes
        onActivationCallback: {
              manager.saveScreenShot(props.attributeNo)
        }

    }

    //    Rectangle{
    //        id:propertyList
    //        width: parent.width*.25
    //        anchors{
    //            top:parent.top
    //            left:parent.left
    //            bottom:parent.bottom
    //            leftMargin: scaleX(10)
    //        }
    //        color:"green"
    //        StyledText{
    //            id:propLabel
    //            text:"Attribtes"
    //            anchors.top: parent.top
    //            anchors.horizontalCenter: parent.horizontalCenter
    //        }

    //        ListView{
    //            id:propView
    //            spacing:scaleY(2)
    //            anchors{
    //                top:propLabel.bottom
    //                left:parent.left
    //                right:parent.right
    //                bottom:parent.bottom
    //            }
    //            model:screenshotAttributes
    //            delegate: StyledButton{
    //                label: attributeType
    //                anchors{
    //                    left:parent.left
    //                    right:parent.right
    //                }

    //                onActivated: {
    //                    manager.saveScreenShot(attributeNo)
    //                    manager.cleanupScreenie()
    //                    slideOut.source=""
    //                }
    //            }
    //        }
    //    }

}
