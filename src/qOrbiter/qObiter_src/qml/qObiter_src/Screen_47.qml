import QtQuick 1.0
import "components"

Rectangle {

    width: 800
    height: 480
    color: "slategrey"
    clip: true

    Style{id:style}

    Component {
             id: contactDelegate
             Item {
                id:mainItem
                 width: 156;
                 height: 100

                 Rectangle {
                     id:frame
                     width: mainItem.width
                     height: mainItem.height
                     anchors.centerIn: mainItem
                     border.width: 1
                     color: "floralwhite"
                     border.color: "black"

                     MouseArea{
                         anchors.fill: frame
                         onClicked: console.log(index + ":"+ name)
                     }

                         Image { source:"image://datagridimg/"+size ; height: 100; width: 156; anchors.centerIn: parent}
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
        x: 10
        y: 7
        model:dataModel
        delegate: contactDelegate
        highlight: Rectangle { color: "lightblue"; radius: 5 }
        highlightFollowsCurrentItem: true
        focus: true
        width: 609
        height: 400
        clip: true

        contentItem.clip: true
        cellWidth: 156
        cellHeight: 100

     Connections{
         target:dataModel
         onDataChanged:console.log("something changed: ")
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
               onClicked:gotoQScreen("Screen_1.qml")
                     }
             }

        }
     }

     AttributeSelector {
         id: attributeselector1
         x: 619
         y: 7
         width: 125
         height: 400
     }
}
