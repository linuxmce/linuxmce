import QtQuick 1.0

Rectangle {

    width: 640
    height: 480

    Style1{id:style}


    GridView {
        id: grid_view1
        x: 10
        y: 7
        model:SampleListModel{}
        delegate: Column
                {
        spacing:5
        id:layoutcol
        Text {text: name; anchors.horizontalCenter: parent.horizontalCenter; font.pointSize: 12 }
        Image {source: img; height: 100; width: 75; anchors.horizontalCenter: parent.horizontalCenter
        }

    }

        width: 620
        height: 395
        contentItem.pos.x: 2
        contentHeight: -6
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
