import QtQuick 1.0


Rectangle {
    id:assignscreenshot
    width: scaleX(65)
    height: scaleY(75)
    radius: 5
    color: "slategrey"

    Image {
        id: screenshotImage
        source: "image://listprovider/screenshot/"
        height: 400
        fillMode: Image.PreserveAspectFit
        width: 400

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
