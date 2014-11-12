import QtQuick 1.1
import org.linuxmce.enums 1.0

Image {
    id: nowplayingimage

    property string aspectRatio:dcenowplaying.aspect
    height:scaleY(30)
    width:height
    source: ""
    opacity: 1
    scale:1
    Connections    {
        target: dcenowplaying
        onImageChanged: nowplayingimage.source = "image://listprovider/updateobject/"+securityvideo.timestamp
    }
//    StyledText{
//        anchors.centerIn: parent
//        text:dcenowplaying.imageAspectRatio
//        fontSize: scaleY(10)
//        color:"white"
//    }
    states: [
        State {
            name: "wide"
            when:dcenowplaying.aspect==="WIDE"
            PropertyChanges {
                target: nowplayingimage
                height:manager.isProfile? scaleX(55) : scaleX(25)
                width:manager.isProfile ? scaleY(55)   : scaleY(25)
            }
        },
        State {
            name: "album"
            when:dcenowplaying.aspect==="ALBUM"
            PropertyChanges {
                target: nowplayingimage
                height:manager.isProfile? scaleX(55) : scaleY(25)
                width:height
            }
        },
        State {
            name: "poster"
            when:dcenowplaying.aspect==="POSTER"
            PropertyChanges {
                target: nowplayingimage
                height:manager.isProfile? scaleX(15) : scaleX(25)
                width:manager.isProfile ? scaleY(25)   : scaleY(25)
            }
        }
    ]
}
