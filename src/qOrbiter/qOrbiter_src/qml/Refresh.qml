import QtQuick 1.1

Item {
    height: manager.appHeight
    width: manager.appWidth
    Rectangle{
        anchors.fill: parent
        color:"black"
    }

    function screenchange(screen){
console.log("Requested screen "+screen)
    }

    Text{
        anchors.centerIn: parent
        color:"white"
        font.weight: Font.DemiBold
        text:"Reloading Component Cache, please be patient."
        font.pointSize: 42
        width: parent.width /2
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }
}
