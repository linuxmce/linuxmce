import QtQuick 2.2
import QtGraphicalEffects 1.0
import AudioVisual 1.0
import "."
import "components"
/*! This File is designed to be the main layout that can be switched in and out for various forms */
Item {
    id:layout
    anchors.fill: qmlRoot

    Rectangle{
        anchors.fill: parent
        color:"black"
    }
    property bool uiOn:true
    function raiseNavigation(raise){
        uiOn=raise;
    }

    QmlPictureFrame {
        id: qmlPictureFrame
        MouseArea{
            anchors.fill: parent
            onClicked: uiOn=!uiOn
        }
    }


    PageLoader {
        id: pageLoader
        anchors{
            top:header.bottom
            left:layout.left
            right:layout.right
            bottom:footer.top
        }
    }

    DefaultHeader {
        id: header

    }

    Footer {
        id: footer
    }

}
