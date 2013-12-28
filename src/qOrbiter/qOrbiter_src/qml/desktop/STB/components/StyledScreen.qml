import QtQuick 1.1

Item {
    anchors{
        top:pageLoader.top
        left:pageLoader.left
        right:pageLoader.right
        bottom: pageLoader.bottom
    }

    Rectangle{
        anchors.fill: parent
        gradient: appStyle.contentGradient
        opacity: .25
    }

}
