import QtQuick 1.0
import "../components"
/*
screen params for this screen are

*/
StyledScreen{
    id:quadcameraview

    onScreenOpening: {
        currentInterval=glScreenSaver.interval
        glScreenSaver.interval=300000000
    }

    onScreenClosing: {
        glScreenSaver.interval=currentInterval
        currentInterval=0
    }

    property int camera:screenparams.getParam(2)
    property variant cameraList:[]
    property int tick:0
    property int viewPortHeight:manager.appWidth /2.75
    property int viewPortWidth:manager.appWidth / 2.75
    property int currentInterval:0


    Component.onCompleted: {

        cameraList =(screenparams.getParam(103)).split(",")
        for(var c in cameraList){
            if(Number(cameraList[c])==0){
                cameraList.splice(c,1)
                console.log("removing!")
            }
        }
        cameraLayout.model=cameraList.length
        picTimer.running=true
    }

    Timer{
        id:picTimer
        repeat: true
        interval: 2000
        triggeredOnStart: true
        running: false
        onTriggered:{
            // "image://listprovider/securityimage/"+cameraList[tick]+"/"+securityvideo.timestamp

            for (var cam in cameraList){
                console.log("requesting new security image for "+cameraList[cam])
                if(Number(cameraList[cam]!=0))
                    manager.requestSecurityPic(cameraList[cam], 640, 480)
                else
                    console.log("Invalid Camera")
            }
        }
    }



    Panel{
        id:securitycamrect
        anchors.fill: parent

        ListView{
            id:cameraLayout
            // model: cameraList.length
            anchors.fill: parent
            delegate:Item{
                height: viewPortHeight
                width: parent.width

                function updateSource(url){

                }

                property int camera:cameraList[index]
                StyledHeader{
                    useClose: false
                    labelText: "Camera::"+ camera
                    height:undefined
                    anchors{
                        top:parent.top
                        right:parent.right
                        bottom:parent.bottom
                    }
                }
                Image {
                    id: securityimage
                    fillMode: Image.PreserveAspectFit
                    source:  "image://listprovider/securityimage/"+camera+"/"+securityvideo.timestamp
                    anchors.left: parent.left
                    height: parent.height
                    width: parent.width/2
                }
            }
        }

    }


}
