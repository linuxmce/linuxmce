// import QtQuick 1.1 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    width: scaleX(90)
    height: scaleY(45)
    color: "transparent"
    clip:true


    ListView{
        id:coverList
        width: scaleX(90)
        height: scaleY(45)
        orientation:ListView.Horizontal
        preferredHighlightBegin: parent.width/2; preferredHighlightEnd: parent.width/2  //this line means that the currently highlighted item will be central in the view
        // highlightRangeMode: ListView.StrictlyEnforceRange  //this means that the currentlyHighlightedItem will not be allowed to leave the view
         highlightFollowsCurrentItem: true  //updates the current index property to match the currently highlighted item
       // onCurrentIndexChanged: positionViewAtIndex(currentIndex, ListView.Center)
        flickDeceleration: 2000
       // highlightRangeMode: ListView.ApplyRange
        model:dataModel
        highlight:Rectangle{
            height: scaleX(21)
            width: scaleX(21)
            color:"black"
            border.color: "white"
            border.width: 1
        }
        highlightMoveDuration:1000

        delegate: Rectangle{
            height: scaleX(20)
            width: scaleX(20)
            color:"transparent"

            Image
            {
                id: imagerect;
                source:"image://datagridimg/"+id ;
                height: scaleY(18);
                width: scaleX(18);
                anchors.centerIn: parent;
                fillMode: Image.PreserveAspectCrop
                smooth: true
                asynchronous: true
            }

            StyledText
            {
                id:celllabel
                textLabel: name;
                font.pointSize: 12;
                color: "white" ;
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                width: imagerect.width
                font.bold: true
                anchors.top: imagerect.top
                anchors.horizontalCenter: imagerect.horizontalCenter
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    coverList.currentIndex = index
                    setStringParam(4, id);
                }
            }

        }
    }
}
