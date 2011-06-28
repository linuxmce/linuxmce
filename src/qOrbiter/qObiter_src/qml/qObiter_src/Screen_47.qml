import QtQuick 1.0
import "components"

Rectangle {

    width: 800
    height: 480
    color: "slategrey"
    clip: true

    Style1{id:style}

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

                     Image { source:"image://datagridimg/"+size ; height: 100; width: 156; anchors.centerIn: frame}

                        Flow
                        {
                            anchors.fill: frame
                            anchors.margins: 4
                            spacing: 10
                        Text {
                            text: name;
                            opacity: 1;
                            font.pointSize: 12;
                            color: "black" ;
                            wrapMode: "WrapAnywhere"
                            }
                        }
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
        width: 785
        height: 390

        contentItem.clip: true
        cellWidth: 156
        cellHeight: 100
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
               onClicked: dceObject.gotoQScreen("Screen_1.qml")
                     }
             }

        }
     }
}
