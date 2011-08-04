import QtQuick 1.0
import "../components"


Rectangle {
    id:fileviewscreen

    width: style.orbiterW
    height: style.orbiterH
    color: style.bgcolor
    clip: true

    Component {
             id: contactDelegate
             Rectangle {
                id:mainItem
                 width: 175;
                 height: 109
                 color: "transparent"
                 border.color: "black"
                 border.width: 1

                 Rectangle {
                     id:frame
                     opacity: 1
                     width: 156
                     height: 100
                     anchors.centerIn: mainItem


                     color: "floralwhite"
                     border.color: "black"
                     border.width: 2

                     MouseArea{
                         anchors.fill: frame
                         onClicked: setStringParam(4, id)
                     }

                         Image { id: imagerect; source:"image://datagridimg/"+id ; height: 100; width: 156; anchors.centerIn: parent}
                     }

                 Text {
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


    GridView {
        id: grid_view1
        x:20
        anchors.horizontalCenter: fileviewscreen.horizontalCenter
        model:dataModel
        delegate: contactDelegate
        highlight: Rectangle { color: "lightblue"; radius: 5 }
        highlightFollowsCurrentItem: true
        focus: true
        width: parent.width - 10
        height: 500

        clip: true
        contentItem.clip: true
        cellWidth: 175
        cellHeight: 109

        onModelChanged:
        {
            console.log("Grid Item Added!")
        }

        Connections
        {
         target:dataModel
                    }
            }
    Row{
        height: childrenRect.height
        anchors.topMargin: 0
        width: childrenRect.width
        anchors.top: grid_view1.bottom
        anchors.left: grid_view1.left

        ButtonSq{
            height: style.iconHeight
            width: style.iconWidth
            buttontext: "Go Back"

            buttontextbold: true

            MouseArea{
                anchors.fill:parent
                onClicked: goBackGrid()
            }

        }

        AttributeSelector {}

    }

}
