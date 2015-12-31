import QtQuick 1.0
import "../components"

Rectangle {

    width: style.orbiterW
    height: style.orbiterH
    color: "slategrey"
    clip: true



    Component {
             id: contactDelegate
             Item {
                id:mainItem
                 width: 156;
                 height: 100

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
        width: 669
        height: 539
        clip: true
        contentItem.clip: true
        cellWidth: 156
        cellHeight: 100

        onModelChanged: {console.log("Grid Item Added!")}


     Connections{
         target:dataModel

     }

    }


    Rectangle{
        width: childrenRect.width
        height: parent.height
        color: "floralwhite"
        anchors.top: grid_view1.bottom
   Row{
       id:options
       height:80
       width: parent.width - 5



       ButtonSq{
           height: 45
           width:  45
           buttontext: "opt"
           color: "lightblue"
                }

       ButtonSq{
           height: 45
           width:  45
           buttontext: "opt1"
           color: "lightblue"
                }

       ButtonSq{
           height: 45
           width:  45
           buttontext: "Home"
           color: "lightblue"

           MouseArea{
               anchors.fill: parent
               onClicked:setCurrentScreen("Screen_1.qml")
                     }
             }

        }
     }

     AttributeSelector {
         id: attributeselector1
         x: 675
         y: 19
         width: 125
         height: 400
     }
}
