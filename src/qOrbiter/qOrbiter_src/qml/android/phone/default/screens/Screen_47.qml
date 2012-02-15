import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs



Rectangle {
    id:fileviewscreen
    width: appW
    height: appH
    color: "transparent"
    clip: true
    Connections
    {
        target: filedetailsclass
        onShowDetailsChanged:
        {
            MyJs.createFileDetails()

        }
    }

    Component
    {
        id: contactDelegateList
        Item {
            width: scaleX(99)
            height: scaleY(8)
            Rectangle {
                id: background
                x: 2; y: 2; width: parent.width - x*2; height: parent.height - y*2
                color: "floralwhite"
                border.color: "black"
                radius: 5
            }
            MouseArea {
                anchors.fill: parent
                onClicked: setStringParam(4, id)
            }
            Row {
                id: topLayout
                x: 10; y: 10; height: imagerect.height; width: parent.width
                spacing: 10

                Image
                {
                    id: imagerect;
                    source:""
                    height: scaleX(10);
                    width: scaleY(10);
                    fillMode: Image.PreserveAspectFit;
                }

                Column {
                    width:parent.width
                    height: parent.height
                    spacing:5
                    Text {
                        text: name;
                        opacity: 1;
                        font.pointSize: scaleY(2);
                        color: "black" ;
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        //anchors.fill: parent
                        font.bold: false
                    }
                }

            }
        }
    }


    ListView {
        id: list_view1
        width: scaleX(100)
        height:scaleY(75)
        model:dataModel
        delegate: contactDelegateList
        clip: true
        Component.onCompleted: dcerouter.prepareFileList(5)
    }


    Row
    {
        height: childrenRect.height
        anchors.topMargin: 0
        width: childrenRect.width
        anchors.top: list_view1.bottom
        anchors.left: list_view1.left
        ButtonSq
        {
            height: style.iconHeight
            width: style.iconWidth
            buttontext: "Go Back"

            buttontextbold: true

            MouseArea
            {
                anchors.fill:parent
                onClicked: goBackGrid()
            }
        }
        AttributeSelector {}
    }
}
