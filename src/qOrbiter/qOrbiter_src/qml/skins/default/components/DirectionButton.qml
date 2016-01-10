import QtQuick 2.2
import QtGraphicalEffects 1.0

Item {
    id:arrow_root
    width: 150
    height: width
    property alias imageSource:img.source
    signal activated()

    DropShadow {
        anchors.fill: img
        horizontalOffset: 3
        verticalOffset: 3
        radius: 8.0
        samples: 24
        color:ms.pressed ? "#ffffff" : "#43acff"
        source: img
        rotation: img.rotation
    }

    Image {
        id: img
        height: arrow_root.height
        width:arrow_root.width
        fillMode: Image.PreserveAspectFit
        source: "../images/vertical_arrow.png"
        visible: false
        smooth: true
    }

    MouseArea{
        id:ms
        anchors.fill: parent
        onClicked: activated()
    }
}
