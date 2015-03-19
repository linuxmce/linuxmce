import QtQuick 2.1
import "../components"
StyledScreen {
    id:screen1
    focusTarget: screen1

    Component.onCompleted: {
        manager.setBoundStatus(true)
        if(dcenowplaying.b_mediaPlaying )
        {
            console.log("going to screen "+ dcenowplaying.qs_screen)
            manager.setCurrentScreen(dcenowplaying.qs_screen)
        }
    }

    Item{
        height: parent.height *.75
        width: parent.width *.75
        anchors.centerIn: parent
        Rectangle{
            anchors.fill: parent
            color:"black"
            opacity: .55
        }

        Column{
            width:childrenRect.width
            anchors.horizontalCenter: parent.horizontalCenter
            StyledText{
                text: "Screen Name:" +screenInfo.primaryScreen.screenName
                color: "white"
            }
            StyledText{
                text: "Size:" +screenInfo.primaryScreen.height+"h x \n"+screenInfo.primaryScreen.width+"w"
                color: "white"
            }
            StyledText{
                text: "Physical DPI:" +screenInfo.primaryScreen.physicalDpi
                color: "white"
            }
            StyledText{
                text: "Logical DPI:" +screenInfo.primaryScreen.logicalDpi
                color: "white"
            }
            StyledText{
                text: "Native Orientation:" +screenInfo.primaryScreen.nativeOrientation
                color: "white"
            }
            StyledText{
                text: "Primary Orientation:" +screenInfo.primaryScreen.primaryOrientation
                color: "white"
            }
            StyledText{
                text: "Physical Height:" +screenInfo.primaryScreen.heightMM+"mm"
                color: "white"
            }
            StyledText{
                text: "Physical Width:" +screenInfo.primaryScreen.widthMM+"mm"
                color: "white"
            }
        }

    }

    Connections{
        target: dcenowplaying
        onScreenTypeChanged:manager.setCurrentScreen(dcenowplaying.qs_screen)
    }



}
