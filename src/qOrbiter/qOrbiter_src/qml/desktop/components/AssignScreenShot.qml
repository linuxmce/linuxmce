import QtQuick 1.0


Rectangle {
    id:assignscreenshot
    width: scaleX(65)
    height: scaleY(75)
    radius: 5
    color: "slategrey"
      property ListModel thumbmodel: ListModel{}
    Component.onCompleted: worker.sendMessage({msg: "init", arg: top.list});

    //note the import of the js as a worker script and not using the 'import' command used with qtquick
    WorkerScript {
        id: worker
        source: "../js/ThreadedAttributes.js"
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

        height: 50
        width: parent.width-1
        Text {
            id: propertystring
            text: qsTr("type of object")
        }
        Text {
            id: propertystring2
            text: qsTr("object value")
        }
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

        ListView{
            id:propertydisplay
            model:assignscreenshot.thumbmodel
            delegate: thumbdelegate
            anchors.fill: attributes
        }
    }

    Image {
        id: screenshotImage
        anchors.left: attributes.right
        anchors.leftMargin: 10
        source: "image://listprovider/screenshot/"
        height: parent.height *.65
        fillMode: Image.PreserveAspectFit
        width: parent.width*.50

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
            onClicked: assignscreenshot.destroy()
        }
    }

}
