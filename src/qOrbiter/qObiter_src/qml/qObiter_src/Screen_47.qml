import QtQuick 1.0

Rectangle {
    signal gotoScreen (String screen)
    width: 640
    height: 480

    Style1{id:style}

    GridView {
        id: grid_view1
        x: 10
        y: 7
        width: 620
        height: 395
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
               onClicked: gotoScreen("1")
                }
   }

}
}
