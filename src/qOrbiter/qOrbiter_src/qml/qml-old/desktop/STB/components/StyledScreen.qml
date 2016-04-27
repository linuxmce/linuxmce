import QtQuick 1.1

Item {
    property string screenName:"lorem ipsum foo"
    property string screenDescription:"basic description"

    anchors{
        top:pageLoader.top
        left:pageLoader.left
        right:pageLoader.right
        bottom: pageLoader.bottom
    }

//    Rectangle{
//        anchors.fill: parent
//        gradient: appStyle.contentGradient
//        opacity: .25
//    }

    MouseArea{
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            if(mouse.button===Qt.LeftButton){
                if(pageLoader.activeFocus){
                     ftr.forceActiveFocus()
                }

            } else if (mouse.button===Qt.RightButton){
                if(ftr.isActive){
                     pageLoader.forceActiveFocus()
                }

            }
        }
    }

}
