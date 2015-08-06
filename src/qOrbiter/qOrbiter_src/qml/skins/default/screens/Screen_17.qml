import QtQuick 2.2
import org.linuxmce.enums 1.0
import "../components"
import "../."

StyledScreen {
    id:multi_camera_view
    screen:qsTr("Multi Camera View")

    property variant cameraList:[]
    property int tick:0

    onScreenClosing: {
        manager.setActiveRoom(roomList.currentRoom, roomList.currentEA);
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
    ContentHeader {
        id: hdr
        label:qsTr("Security Cameras")
    }


    GridView{
        id:cameraLayout
        model:cameraList.length
        currentIndex: 0
        anchors{
            top:hdr.bottom
            left:parent.left
            right:parent.right
            bottom:parent.bottom
            topMargin: 5
        }
        cellHeight:cameraLayout.height/2
        cellWidth:cameraLayout.width/2

        delegate:Item{
            height: cameraLayout.height*.48
            width:cameraLayout.width*.48
            property int camera:cameraList[index]
            Rectangle{
                anchors.fill: parent
                color:"black"
            }

            //  fillColor: index===cameraLayout.currentIndex ? "green" :"white"
            //  headerTitle:"Tap to view Camera "+ camera
            Image {
                id: securityimage
                clip:true
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                source:  "image://listprovider/securityimage/"+camera+"/"+securityvideo.timestamp

                height: parent.height
            }
            ContentHeader{
                label: qsTr("Camera # %1").arg(camera)
                opacity: .65
            }
            MouseArea{
                anchors.fill: parent
                onClicked: { manager.requestSingleView(camera) }
            }

        }
    }

}
