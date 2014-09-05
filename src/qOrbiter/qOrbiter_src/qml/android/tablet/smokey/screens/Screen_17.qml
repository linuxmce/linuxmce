import QtQuick 1.1
import "../components"

StyledScreen{
    id:multiCameraView
    property variant cameraList:[]
    property int tick:0

    onTickChanged: {
        if(tick > cameraList.length-1)
            tick=0
    }

    Timer{
        id:picTimer
        repeat: true
        interval: 2000
        triggeredOnStart: true
        running: true
        onTriggered:{
            if(cameraList[tick] ==="" || cameraList===undefined){
                tick++
                cameraLayout.currentIndex=0
                return;
            }


            // "image://listprovider/securityimage/"+cameraList[tick]+"/"+securityvideo.timestamp
            console.log("requesting new security image")
            manager.requestSecurityPic(cameraList[tick], 640, 480)
            tick++
            cameraLayout.currentIndex=tick

        }
    }

    StyledHeader{
        id:hdr
        useClose: false
        labelText: cameraList.toString() + "::"+cameraList.length+" cameras. current camera is "+cameraList[tick]
    }

    Component.onCompleted: {
        cameraList =(screenparams.getParam(103)).split(",")
        cameraList.unshift("")
    }

    GridView{
        id:cameraLayout
        model: cameraList.length
        anchors{
            top:hdr.bottom
            left:parent.left
            right:parent.right
            bottom:parent.bottom
        }
        cellHeight:400
        cellWidth:400

        delegate:Item{
            height: cameraLayout.currentIndex=== index ? 400 :200
            width: cameraLayout.currentIndex=== index ? 400 : 200
            property int camera:cameraList[index]
            Image {
                id: securityimage
                fillMode: Image.PreserveAspectFit
                source:  "image://listprovider/securityimage/"+camera+"/"+securityvideo.timestamp
                anchors.fill: parent
            }
            StyledHeader{
                useClose: false
                labelText: camera
            }

        }
    }

}
