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
        interval: 1000
        triggeredOnStart: true
        running: true
        onTriggered:{
            console.log("requesting new security image for "+cameraList[cameraLayout.currentIndex])
            manager.requestSecurityPic(cameraList[cameraLayout.currentIndex], 640, 480)
            if(cameraLayout.currentIndex===cameraLayout.count-1){
                cameraLayout.currentIndex=0
            }else {
               cameraLayout.currentIndex++
            }


        }
    }


    Component.onCompleted: {
        var prep =  (screenparams.getParam(103).split(","))
        var tmpArry = new Array
        for(var c in prep){
            var t = Number(prep[c])
            if(t!=0){
                tmpArry.push(prep[c])
            }
        }

        cameraList=tmpArry
        picTimer.running=true
    }

    GridView{
        id:cameraLayout
        model:cameraList.length
        currentIndex: 0
        anchors{
            top:parent.top
            left:parent.left
            right:parent.right
            bottom:parent.bottom
        }
        cellHeight:cameraLayout.height/2
        cellWidth:cameraLayout.width/2

        delegate:Panel{
            height: cameraLayout.height*.48
            width:cameraLayout.width*.48
            property int camera:cameraList[index]
            fillColor: index===cameraLayout.currentIndex ? "green" :"white"
            headerTitle:"Tap to view Camera "+ camera
            Image {
                id: securityimage
                anchors.top: headerRect.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.topMargin: 5
                fillMode: Image.PreserveAspectFit
                source:  "image://listprovider/securityimage/"+camera+"/"+securityvideo.timestamp

                height: parent.height-headerRect.height
            }

        }
    }

}
