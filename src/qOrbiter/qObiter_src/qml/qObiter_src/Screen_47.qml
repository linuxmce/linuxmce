import QtQuick 1.0

Rectangle {

    width: 640
    height: 480

    Style1{id:style}

    Component {
             id: contactDelegate
             Item {
                id:mainItem
                 width: 180; height: 100

                 Rectangle {
                     id:frame
                     width: mainItem.width
                     height: mainItem.height
                     anchors.centerIn: mainItem
                     border.width: 1
                     color: "lightgrey"
                     border.color: "black"

                     Image { source:"image://datagridimg/"+index ; height: 90; width: 160; anchors.centerIn: frame}

                        Rectangle{
                            id: layer
                            anchors.centerIn: frame
                            color: "navajowhite"
                            opacity: .2
                            width: 160
                            height: 90

                        }
                        Flow
                        {
                            anchors.fill: frame
                            anchors.margins: 4
                            spacing: 10
                        Text { text: name;
                            opacity: 1;
                            font.pointSize:
                                12; color:
                                "black" ;
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
        width: 618
        height: 395
        contentItem.clip: true
        cellWidth: 180
        cellHeight: 100


    }


   Row{
       id:options
       height:80
       width: parent.width - 5
       anchors.top: grid_view1.bottom

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
