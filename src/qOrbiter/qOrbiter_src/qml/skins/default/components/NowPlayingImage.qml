import QtQuick 2.2
import org.linuxmce.enums 1.0
import org.linuxmce.screens 1.0
import QtGraphicalEffects 1.0
import "../."
Item{
    id:imgContainer
    height:0
    width: 0


    property string s_aspect: dcenowplaying.aspect
    property double dvdPosterRatio:955/1080
    property double hdPosterRatio:755/1080
    property double cdCoverRatioFront:1080/1080
    property double cdCoverRatioBack:1080/1264
    property double vcdRatio:1080/1080
    property double vhsRatio:1280/620
    Rectangle{
        anchors.fill: parent
        color:"black"
    }

    onS_aspectChanged: {
        nowplayingimage.source="image://listprovider/updateobject/"+securityvideo.timestamp
        console.log("checking mediatype "+manager.i_current_mediaType)

        if(nowplayingimage.source!==""){
            console.log("invalid image")
            state="audio-default"
            return;
        }


        switch(manager.i_current_mediaType){
        case MediaTypes.LMCE_StoredAudio:
            console.log("LMCE_StoredAudio::ASPECT RATIO==>"+dcenowplaying.aspect)
            state="wideImage"
            break;
            
        case MediaTypes.LMCE_StoredVideo:
            
            if(dcenowplaying.aspect=="poster"){
                console.log("LMCE_StoredVideo::ASPECT RATIO==>"+dcenowplaying.aspect)
                imgContainer.state="posterImage"
            } else if (dcenowplaying.aspect=="wide"){
                console.log("LMCE_StoredVideo::ASPECT RATIO==>"+dcenowplaying.aspect)
                imgContainer.state="wideImage"
            } else if(dcenowplaying.aspect=="square"){
                console.log("LMCE_StoredVideo::ASPECT RATIO==>"+dcenowplaying.aspect)
                imgContainer.state="squareImage"
            }
            break;
        }


    }
    
    
    Connections{
        target:dcenowplaying
        onImageChanged:refreshtimer.start()
    }
    StyledText{
        id:noImageLabel
        text:qsTr("No Image Available")
        anchors.centerIn: parent
    }
    Image {
        id: nowplayingimage
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        source: "image://listprovider/updateobject/"+securityvideo.timestamp
        // source: "image://listprovider/updateobject/"+securityvideo.timestamp
        //  anchors.horizontalCenter: parent.horizontalCenter
       // sourceSize.height: parent.height
       // sourceSize.width: parent.width
        smooth: true
        visible: source == undefined ? false : true
        onSourceChanged: {
            console.log("nowplayingimage::ASPECT==>"+dcenowplaying.aspect)
            console.log("Source Size ==>"+sourceSize)
        }

    }
    
    Timer{
        id:refreshtimer
        interval: 500
        onTriggered: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
        running: true     
    }
    
    states: [
        State {
            name: "wideImage"
            PropertyChanges {
                target: imgContainer
                height:hdPosterRatio*width
                width: manager.isProfile ? Style.scaleX(33) : Style.scaleX(30)
            }
        },
        State {
            name: "squareImage"
            when:manager.i_current_mediaType == MediaTypes.LMCE_StoredAudio
            PropertyChanges {
                target: imgContainer
                height:Style.remote_albumHeight
                width: Style.remote_albumWidth                
            }
        },
        State{
            name:"posterImage"
            PropertyChanges{
                target: imgContainer
                height:Style.remote_posterHeight
                width: Style.remote_posterWidth
            }
        },
        State{
            name:"audio-default"
            PropertyChanges{
                target: imgContainer
                height: manager.isProfile ? Style.scaleX(35) : Style.scaleX(25)
                width:imgContainer.height
            }

        }
        
    ]
}
