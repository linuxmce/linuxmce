import QtQuick 1.1

Image {
    id: nowplayingimage

    property string aspectRatio:dcenowplaying.aspect
    height:scaleY(30)
    width:scaleX(30)
    source: ""
    opacity: 1
    scale:1

    StyledText{
        anchors.centerIn: parent
        text:dcenowplaying.imageAspectRatio
        fontSize: scaleY(10)
        color:"white"
    }
    states: [
        State {
            name: "wide"
            when:dcenowplaying.aspect==="WIDE"
            PropertyChanges {
                target: nowplayingimage
                height:manager.b_orientation? scaleX(55) : scaleX(25)
                width:manager.b_orientation ? scaleX(55)   : scaleY(25)
            }
        },
        State {
            name: "album"
            when:dcenowplaying.aspect==="ALBUM"
            PropertyChanges {
                target: nowplayingimage
                height:manager.b_orientation? scaleX(55) : scaleY(25)
                width:manager.b_orientation ? scaleX(55)   : scaleY(35)
            }
        },
        State {
            name: "poster"
            when:dcenowplaying.aspect==="POSTER"
            PropertyChanges {
                target: nowplayingimage
                height:manager.b_orientation? scaleX(55) : scaleX(25)
                width:manager.b_orientation ? scaleX(55)   : scaleY(25)
            }
        }
    ]
}
