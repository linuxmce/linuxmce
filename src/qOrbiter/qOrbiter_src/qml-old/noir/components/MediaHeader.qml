// import QtQuick 2.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.0

Item{
    id:hdrContainer
    property bool currentState:true
    height: childrenRect.height
    width: childrenRect.width
    state: currentState ? "show" : "hide"  //currentState === true ? "SHOW" : "HIDE"
    focus:false
    onStateChanged: console.log(state)
    onCurrentStateChanged: {
        if(hdrContainer.state==="hide")
        {
            hdrContainer.state="show"
        }
        else
        {
            hdrContainer.state = "hide"
        }
    }

    states: [
        State {
            name: "show"
            AnchorChanges {
                target: hdrContainer
                //  anchors.bottom: undefined
                anchors.top: parent.top

            }
        },
        State {
            name: "hide"
            AnchorChanges {
                target:hdrContainer
                //  anchors.top: undefined
                anchors.bottom: parent.top
            }
        }
    ]
    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration: 750
                targets: hdrContainer
            }
        }
    ]

    Rectangle {
        id:mediaHeader
        width: manager.appWidth
        height: manager.appHeight *.06
        //color: "transparent"
        opacity: .4
        border.color: "white"
        border.width: 1
        gradient:style.generalGradient
    }
    StyledText {
        id: nowPlayingStyledText
        text: dcenowplaying.mediatitle === "" ? dcenowplaying.qs_mainTitle : dcenowplaying.mediatitle
        color: "white"
        font.pointSize: 32
        font.family: "Nimbus Sans L"
        anchors.centerIn: mediaHeader
    }

    StyledText {
        id: mediaClock
        text:dceTimecode.qsCurrentTime+ " :: " + dceTimecode.qsTotalTime
        font.family: "Nimbus Sans L"
        color: "white"
        font.pointSize: 28
        anchors.right: mediaHeader.right
        anchors.verticalCenter: mediaHeader.verticalCenter
    }
}


