import QtQuick 1.0
import "../components"

Rectangle {
    id:stage


    signal swapStyle()
    height: style.orbiterH
    width: style.orbiterW
    color: "transparent"

    Clock{
        anchors.right: parent.right
        verticalMirror: true
    }

    // Upper left menu
    Image {
        source: "../img/common/timepanel_mid-vh.png"
        width: scaleX(35.23) // 451
        height:  scaleY(3.75) // 27
        smooth:  true
        Text {
            text: "Home"
            anchors.leftMargin: scaleX(.78)
            font.family: aeonRss.name;
            font.pixelSize: parent.height*.7;
            color: "#e5e5e5";
            smooth: true
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
        }
        MouseArea {
            anchors.fill: parent
            onClicked:  gotoQScreen("Screen_1.qml")
        }
    }

    // Build the background
    Image{
        y: scaleY(43.75) // 315
        width: parent.width
        height: scaleY(56.25) // 405
        source: "../img/showcase/lite/showcase_floor.png"
    }
    Image{
        x: scaleX(11.25) // 144
        y: scaleY(80.97) // 583
        width: scaleX(77.58) // 993
        height: scaleY(19.03)  // 137
        source: "../img/showcase/lite/showcase_spot.png"
    }
    Image{
        width: scaleX(36.25) // 464
        height: parent.height
        source: "../img/showcase/showcase_vignette.png"
    }
    Image{
        x: scaleX(63.75) // 816
        width: scaleX(36.25) // 464
        height: parent.height
        source: "../img/showcase/showcase_vignette-r.png"
        //mirror: true // not supported?
    }

    Component {
        id: appDelegate
        Item {
            id: wrapper
            width: scaleX(15.63) // 200;
            height: scaleY(38.2) // 275
            scale: PathView.iconScaling
            z: PathView.iconZ
//            Rectangle{
            Image {
                anchors.fill: parent
                clip: true
//                radius:7
                smooth: true
                transform: Rotation {
                    origin.x: wrapper.width/2; origin.y: wrapper.height/2
                    axis.x: 0; axis.y: 1; axis.z: 0     // rotate around y-axis
                    angle: PathView.iconAngle
                }

//                Image {
                    id: myIcon
//                    smooth: true
                    source:"image://datagridimg/"+id ;
//                    anchors.fill: parent
                    fillMode: Image.PreserveAspectCrop;
                    asynchronous: true


                }
//            }

            Text {
                anchors { top: myIcon.bottom; horizontalCenter: wrapper.horizontalCenter }
                text: name
                smooth: true
                visible: wrapper.PathView.isCurrentItem
            }

            MouseArea {
                anchors.fill: wrapper
                onClicked: {
                    //setStringParam(4, id)
                    if(PathView.isCurrentItem){
                        console.log("clicked",id)
                    } else {
                        path_view1.currentIndex = index
                    }
                }
            }
        }
    }

    PathView {
        id: path_view1
        y: scaleY(55)
        //z: 1
        width: scaleX(80)
        height: scaleY(30)
        //anchors.topMargin: 40
        anchors.horizontalCenter: parent.horizontalCenter
        model:dataModel
        focus: true
        //clip: true

        interactive: true
        Keys.enabled: true
        //anchors.fill: parent
        //offset: 2
        pathItemCount: 13;
        //highlight: appHighlight

        preferredHighlightBegin: 0.462
        preferredHighlightEnd: 0.615
        //highlightRangeMode: PathView.StrictlyEnforceRange
        //highlightMoveDuration: 100
        dragMargin: parent.height/3
        delegate: appDelegate
        path: Path {
            startX: scaleX(4.92)
            startY: path_view1.height/2

                PathPercent { value: 0.0 }
                PathAttribute { name: "iconScaling"; value:  0.8 }
                PathAttribute { name: "iconAngle"; value:  50 }
                PathAttribute { name: "iconZ"; value:  0 }

            PathLine { x: (path_view1.width*0.37); y: path_view1.height/2 }
                PathPercent { value: 0.385 }
                PathAttribute { name: "iconScaling"; value:  0.8 }
                PathAttribute { name: "iconAngle"; value:  50 }
                PathAttribute { name: "iconZ"; value:  10 }

            PathLine { x: path_view1.width/2; y: path_view1.height/2 }
                PathPercent { value: 0.462 }
                PathAttribute { name: "iconScaling"; value:  1.0 }
                PathAttribute { name: "iconAngle"; value:  0 }
                PathAttribute { name: "iconZ"; value:  11 }

            PathLine { x: (path_view1.width*0.63); y: path_view1.height/2 }
                PathPercent { value: 0.538 }
                PathAttribute { name: "iconScaling"; value:  0.8 }
                PathAttribute { name: "iconAngle"; value:  -50 }
                PathAttribute { name: "iconZ"; value:  10 }

            PathLine { x: path_view1.width; y: path_view1.height/2 }
                PathPercent { value: 1.0 }
                PathAttribute { name: "iconScaling"; value:  0.8 }
                PathAttribute { name: "iconAngle"; value:  -50 }
                PathAttribute { name: "iconZ"; value:  0 }

        }
        onCurrentIndexChanged: {
            //console.log(dataModel.get(currentIndex));
        }

    }
//    Rectangle{
//        border.color: "black"
//        color: "transparent"
//        width: 200
//        height: stage.height
//        anchors.horizontalCenter: stage.horizontalCenter
//    }
//    Rectangle{
//        border.color: "black"
//        color: "transparent"
//        height: 275
//        width: scaleX(80)
//        //anchors.verticalCenter: parent.verticalCenter
//        anchors.centerIn: parent
//    }

}
