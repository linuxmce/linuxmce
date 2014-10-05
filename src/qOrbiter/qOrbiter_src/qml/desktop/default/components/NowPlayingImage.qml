import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs


Rectangle{
    id:imageholder
    height:childrenRect.height
    width:scaleX(30)
    color: "transparent"

    property variant dvdPosterDimensions:{"w":755, "h":1080 }
    property variant hdPosterDimensions:{"w":955, "h":1080 }
    property variant cdDimensions:{"w":320, "h":230}
    property variant mameArtDimensions:{"w":320, "h":230}

    property double dvdPosterRatio:1080/955
    property double hdPosterRatio:1080/755
    property double cdCoverRatioFront:1080/1080
    property double cdCoverRatioBack:1080/1264
    property double vcdRatio:1080/1080
    property double vhsRatio:1280/620

    Connections{
        target: dcenowplaying
        onImageChanged: refreshtimer.restart()
    }

    //    BorderImage {
    //        id: borderimg
    //        horizontalTileMode: BorderImage.Repeat
    //        source: "../img/icons/drpshadow.png"
    //        height: nowplayingimage.paintedHeight
    //        width: nowplayingimage.paintedWidth
    //        anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
    //        border { left: 10; top: 10; right: 10; bottom: 10 }
    //        smooth: true
    //    }



    Image {
        id: nowplayingimage
         width: dcenowplaying.aspect=="wide"? scaleX(30) : scaleX(25)
          height:dcenowplaying.aspect=="wide"? scaleY(43) : scaleY(45)
        fillMode: Image.PreserveAspectFit
        source: "image://listprovider/updateobject/"+securityvideo.timestamp
        anchors.horizontalCenter: parent.horizontalCenter


        smooth: true
        visible: source == undefined ? false : true
    }
    Timer{
        id:refreshtimer
        interval: 1000
        onTriggered: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
        running: true
    }
    
    Image {
        id: npmask
        source: "../img/icons/transparencymask.png"
        height: nowplayingimage.paintedHeight
        width: nowplayingimage.paintedWidth
        anchors.centerIn: nowplayingimage
        opacity: .75
    }
}
