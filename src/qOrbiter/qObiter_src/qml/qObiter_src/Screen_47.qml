import QtQuick 1.0

Rectangle {

    width: 640
    height: 480

    Style1{id:style}

    Component {
             id: contactDelegate
             Item {

                 width: 75; height: 75
                 Column {
                     anchors.fill: parent

                     Image { source: "../../img/UnknownUser.png"; anchors.horizontalCenter: parent.horizontalCenter; height: 45; width: 35}
                     Text { text: name; anchors.horizontalCenter: parent.horizontalCenter; wrapMode:  "WrapAnywhere" }
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
        width: 454
        height: 395
        cellWidth: 80
        cellHeight: 80

    }

        Rectangle {
            id: rectangle1
            x: 453
            y: 0
            width: 166
            height: 395
            color: "#98b5c0"
        }

        Rectangle {
            id: rectangle2
            x: 453
            y: 0
            width: 166
            height: 16
            color: "#ffffff"

            Text {
                id: text1
                x: 35
                y: 1
                text: "Options"
                font.pixelSize: 12
            }
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
