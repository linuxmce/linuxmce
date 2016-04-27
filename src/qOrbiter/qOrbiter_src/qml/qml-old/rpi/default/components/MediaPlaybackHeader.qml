import QtQuick 2.2
import "../../../skins-common/lib/handlers"
Item {
    id: npHeader
    height: manager.appHeight*.10
    anchors{
        left:template.left
        right:template.right
    }
    Rectangle{
        anchors.fill: parent
        color:appStyle.darkHighlightColor
        opacity: appStyle.appOpacity
    }
    
    Item{
        id:hdrTitle
        width: parent.width/2
        anchors{
            top:parent.top
            left:parent.left
            bottom:parent.bottom
        }
        StyledText {
            id: generaltitle
            width: scaleX(45)
            text:  dcenowplaying.qs_mainTitle
            font.bold: true
            //  wrapMode: "WrapAtWordBoundaryOrAnywhere"
            elide: "ElideRight"
            smooth: true
            fontSize: 40

            color:skinStyle.accentcolor
        }
    }
    Item{
        id:hdrTime
        width: hdrTitle.width
        anchors{
            top:parent.top
            left:hdrTitle.right
            bottom:parent.bottom
        }
        Row{
            id:temporalData
            anchors{
                left:parent.left
                right:parent.right
                bottom:parent.bottom
                top:parent.top
            }
            
            visible: playlist.state === "showing"
            spacing:scaleX(2)
            StyledText {
                id: updating_time
                text: dceTimecode.qsCurrentTime
                fontSize:32
                color:"white"
                anchors.verticalCenter: parent.verticalCenter
            }
            
            StyledText {
                text: qsTr("Of")
                fontSize:32
                color:"white"
                font.bold: true
                anchors.verticalCenter: parent.verticalCenter
            }
            
            StyledText {
                id: totalTime
                text: dceTimecode.qsTotalTime
                fontSize:32
                color:"white"
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
    
    states: [
        State {
            name: "visible"
            when:playlist.state==="showing"
            AnchorChanges{
                target: npHeader
                anchors.bottom: undefined
                anchors.top:template.top

            }
            PropertyChanges {
                target: npHeader
                anchors.bottomMargin: 0
                opacity:1

            }
        },
        State {
            name: "retracted"
            when:playlist.state==="hidden"
            AnchorChanges{
                target: npHeader
                anchors.top: undefined
                anchors.bottom: template.top
            }
            PropertyChanges {
                target: npHeader
                anchors.bottomMargin: 20
                opacity:0
            }
        }
    ]
}
