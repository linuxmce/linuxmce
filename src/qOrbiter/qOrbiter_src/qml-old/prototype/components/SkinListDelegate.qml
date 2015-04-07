import QtQuick 1.1

Item {
    id: skindelegate
    height: manager.appHeight*.30
    width:  manager.appWidth /2
    clip:true
    Rectangle{
        anchors.fill: parent
        color: "black"
        border.color: "white"
    }

    Column{
        height: parent.height
        width: parent.width
        spacing: 1
        StyledText {

            id: nameskin
            text: "Name:" + name + ". Version:" + version
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            color: "white"
        }

        StyledText {

            id: skinDescription
            text: description
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            color: "white"
        }

        StyledText {

            id: skincreator
            text: "Created By:" + creator
            wrapMode: "WrapAnywhere"
            color: "white"
        }

        StyledText {

            id: skinvariant
            text: "Variation for: " + variation
            wrapMode: "WrapAnywhere"
            color: "white"
        }
    }

    MouseArea{
        anchors.fill: parent
        onClicked:{
            setActiveSkin(name)
        }
    }
}




