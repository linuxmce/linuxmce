import QtQuick 1.1

Item {
    id:qmlroot
    anchors.fill: parent
    focus:true
    Keys.onReleased: {
        console.log(event.key)
        event.accepted = true
    }

    Rectangle{
        id:fil
        anchors.fill: parent
        color:"darkblue"
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
        height: childrenRect.height
        width:parent.width
        spacing:2
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
