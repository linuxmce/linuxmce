import QtQuick 1.1

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
        isUsingIndex: true
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


}
