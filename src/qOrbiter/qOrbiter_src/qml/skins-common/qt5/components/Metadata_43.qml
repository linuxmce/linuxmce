import QtQuick 2.2

Item{
    id:textrect
    visible: true
    height: textCol.height
    width: parent.width
    Rectangle{
        anchors.fill: parent
        color:"black"
        opacity:.65
    }

    clip:true
    Column{
        id:textCol
        spacing: scaleY(.5)
        width: parent.width
        anchors.left: parent.left
        anchors.leftMargin: scaleY(1)
        height: childrenRect.height
        StyledText {
            id: video_title
            width:parent.width
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            text:dcenowplaying.episode =="" ? qsTr("Stream Title: ")+ dcenowplaying.qs_mainTitle :qsTr("Program: ")+ dcenowplaying.tvProgram
            font.italic: false
            smooth: true
            fontSize: scaleY(4)
            visible:  dcenowplaying.qs_mainTitle =="" ? false: true

        }
        StyledText {
            id: episode_title
            width:parent.width
            text: qsTr("Episode: ")+ dcenowplaying.episode
            //  wrapMode: "WrapAtWordBoundaryOrAnywhere"
            elide: "ElideRight"
            smooth: true
            fontSize: scaleY(4)
            visible:  dcenowplaying.episode =="" ? false: true
            // color:appStyle.accentcolor
        }


    }
}
