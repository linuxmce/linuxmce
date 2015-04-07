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
             Item {
                id:mainItem
                 width: 179;
                 height: 109

                 Rectangle {
                     id:frame
                     opacity: 1
                     width: mainItem.width
                     height: mainItem.height
                     anchors.centerIn: mainItem
                     color: "floralwhite"
                     border.color: "black"
                     border.width: 2

                     MouseArea{
                         anchors.fill: frame
                         onClicked: console.log(index + ":"+ name+ id)
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

                     }
             }
         }


    GridView {
        id: grid_view1
        x: 6
        y: 7
        model:dataModel
        delegate: contactDelegate
        highlight: Rectangle { color: "lightblue"; radius: 5 }
        highlightFollowsCurrentItem: true
        focus: true
        width: 750
        height: 500
        clip: true
        contentItem.clip: true
        cellWidth: 156
        cellHeight: 100

        onModelChanged: {console.log("Grid Item Added!")}
         Connections{
         target:dataModel
                    }

            }
    Row{
        height: childrenRect.height
        anchors.topMargin: 0
        width: childrenRect.width
        anchors.top: grid_view1.bottom
        anchors.left: grid_view1.left

        AttributeSelector {}

    }








}
