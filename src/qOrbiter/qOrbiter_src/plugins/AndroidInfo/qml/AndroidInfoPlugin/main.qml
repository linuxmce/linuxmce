import QtQuick 1.1

Item {
    id:qmlroot
    anchors.fill: parent
    focus:true
    Keys.onReleased: {

        event.accepted = true
    }

    Rectangle{
        id:fil
        anchors.fill: parent
        color:"black"
    }

   Loader{
    id:pageloader
    source:"Home.qml"
    height: parent.height - nav.height
    width: parent.width
    anchors.top: parent.top
   }

    Row{
        id:nav
        height: childrenRect.height +10
        width:parent.width
        spacing:5
        anchors.bottom: parent.bottom

        Button {
            id: btn
            label:"Home"
            onActivated: pageloader.source = "Home.qml"
        }

        Button{
            label:"Build Information"
            onActivated: pageloader.source = "BuildInfo.qml"
        }

    }

}
