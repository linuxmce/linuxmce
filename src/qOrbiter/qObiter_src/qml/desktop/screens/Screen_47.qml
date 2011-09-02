import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs


Rectangle {
    id:fileviewscreen

    width: style.orbiterW
    height: style.orbiterH
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
        id: contactDelegate
        Rectangle
        {
            id:mainItem
            width: scaleX(20);
            height: scaleY(20)
            color: "transparent"
            border.color: "black"
            border.width: 1

            Rectangle
            {
                id:frame
                opacity: 1
                width: scaleX(19);
                height: scaleY(19)
                anchors.centerIn: mainItem


                color: "floralwhite"
                border.color: "black"
                border.width: 2

                MouseArea
                {
                    anchors.fill: frame
                    onClicked: setStringParam(4, id)
                }

                Image
                {
                    id: imagerect;
                    source:"image://datagridimg/"+id ;
                    height: 100;
                    width: 156;
                    anchors.centerIn: parent;
                    fillMode: Image.PreserveAspectFit;
                    opacity: .5
                    asynchronous: true
                }
            }

            Text
            {
                text: name;
                opacity: 1;
                font.pointSize: 12;
                color: "black" ;
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                anchors.fill: frame
                font.bold: true

            }
        }
    }

    Component
    {
        id: contactDelegateList
        Item {
            width: list_view1.width;
            height: 120
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
                    source:"image://datagridimg/"+id ;
                    height: 100;
                    width: 156;
                    fillMode: Image.PreserveAspectFit;
                }

                Column {
                    width: background.width - imagerect.width - 20;
                    height: imagerect.height
                    spacing:5
                    Text {
                        text: name;
                        opacity: 1;
                        font.pointSize: 12;
                        color: "black" ;
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        //anchors.fill: parent
                        font.bold: true
                    }
                }

            }
        }
    }


    MultiStateFileDisplay{id:grid_view1}

    Component {
             id: appDelegate
             Item {
                 id: wrapper
                 width: scaleX(75); height: scaleY(75)
                 scale:1
                // opacity: path_view1.iconAngle.iconOpacity

                 Image {
                     id: myIcon
                     y: 20; anchors.horizontalCenter: parent.horizontalCenter
                     smooth: true
                     source:"image://datagridimg/"+id ;
                     height: scaleX(25);
                     width: scaleY(25);
                     fillMode: Image.PreserveAspectFit;
                     asynchronous: true
                     transform: Rotation {
                         origin.x: wrapper.width/2; origin.y: wrapper.height/2
                         axis.x: 0; axis.y: 1; axis.z: 0     // rotate around y-axis
                      //   angle: path_view1.iconAngle
                     }

                 }

                 Text {
                     anchors { top: myIcon.bottom; horizontalCenter: parent.horizontalCenter }
                     text: name
                     smooth: true
                     font.pointSize: 12
                     //visible: wrapper.PathView.isCurrentItem
                 }

                 MouseArea {
                     anchors.fill: parent
                     onClicked: setStringParam(4, id)
                 }
             }
         }

         Component {
             id: appHighlight
             Rectangle { width: myIcon.width; height: myIcon.height; color: "lightsteelblue" }
         }

    Row
    {
        height: childrenRect.height
        anchors.topMargin: 0
        width: childrenRect.width
        anchors.top: grid_view1.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        ButtonSq
        {
            height: scaleY(style.iconHeight)
            width: scaleX(style.iconWidth)
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
