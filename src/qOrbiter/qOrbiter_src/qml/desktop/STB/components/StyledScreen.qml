import QtQuick 1.1

Item {
    anchors{
        top:parent.top
        left:parent.left
        right:parent.right
        bottom: parent.bottom
    }

    Rectangle{
        anchors.fill: parent
        gradient: appStyle.contentGradient
        opacity: .25
    }

}
