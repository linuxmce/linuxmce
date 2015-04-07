import QtQuick 1.1
import "../components"

StyledScreen{
    id:multiCameraView
    property variant cameraList:[]
    property int tick:0

    onTickChanged: {
        if(tick > cameraList.length)
            tick=0
    }

    Timer{
        id:picTimer
        repeat: true
        interval: 2000
        triggeredOnStart: true
        running: true
        onTriggered:{
            for (var cam in cameraList){
                console.log("requesting new security image for "+cameraList[cam])
                if(Number(cameraList[cam]!=0))
                    manager.requestSecurityPic(cameraList[cam], 640, 480)
                else
                    console.log("Invalid Camera")
            }

        }
    }

    StyledHeader{
        id:hdr
        useClose: false
        labelText: cameraList.toString() + "::"+cameraList.length+" cameras. current camera is "+cameraList[tick]
    }

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

    GridView{
        id:cameraLayout
        model: cameraList.length
        anchors{
            top:hdr.bottom
            left:parent.left
            right:parent.right
            bottom:parent.bottom
        }
        cellHeight:parent.height*.35
        cellWidth:parent.width *.35

        delegate:Item{
            height: parent.height*.30
            width:parent.width*.30
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
