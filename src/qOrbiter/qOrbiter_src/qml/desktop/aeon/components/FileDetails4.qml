import QtQuick 1.1

Rectangle {
    id: details
    anchors.fill: parent
    color: "transparent"
    opacity: 0
    Behavior on opacity {
        PropertyAnimation{duration:250}
    }
//    Timer {
//        // This should not be needed, but onCompleted for this dialog is being called too soon
//        interval: 500; running: true; repeat: false
//        onTriggered: {
//            synopsisflick.contentHeight=synopsistext.height
//        }
//    }
    Connections{
        target: filedetailsclass
        onImageChanged: {
            coverArt.source = "image://listprovider/filedetailsprovider/"+securityvideo.timestamp
        }
//        onSynopChanged:{
//            synopsisflick.contentHeight=synopsistext.height
//        }
//        onPerformersChanged:{
//            castList.model = filedetailsclass.performers
//        }
    }
    Image {
        anchors.fill: parent
        //source: '../img/common/darkgrey.png'
        Image {
            y: scaleY(11.53) // 83
            width: scaleX(45.31) // 580
            height: scaleY(88.47) // 637
            source: "../img/info/musicinfogradient.png"
            smooth: true
        }
        Image {
            x: scaleX(4.3) // 55
            y: scaleY(22.22) //160
            width: scaleX(28.91) // 370
            height: scaleY(55.56) // 400
            source: '../img/info/musicinfoshadow.png'
            smooth: true
            Image {
                id:coverArt
                anchors.centerIn: parent
                width: parent.width*.8
                height: parent.height*.8
                source:""
                smooth: true
            }
        }

    }

    Image {
        x: scaleX(39.69) // 508
        width: scaleX(60.31) // 772
        height: parent.height
        source: "../img/info/lite/info_rightpaneltop.png"
        smooth: true
    }

    Rectangle {
        color: "transparent"
        x: scaleX(42.97) // 550
        y: scaleY(6.25) // 45
        width: scaleX(54.3) // 695
        height: scaleY(79.03) // 569
        Image {
            y: scaleY(8.33) // 60
            width: parent.width
            height: scaleY(77.78) // 560
            source: '../img/info/info_music_panellines.png'
        }
        Text { // song title
            font.pixelSize: scaleY(9.03) // 65
            text: filedetailsclass.objecttitle
            smooth: true
            color: "white"
            font.family: aeonSettings.name
        }
        Text { // artist
            y: scaleY(9.72) // 70
            font.pixelSize: scaleY(4.17) // 30
            text: filedetailsclass.performerlist
            smooth: true
            color: "white"
            font.family: aeonSettings.name
            Text {
                anchors.bottom: parent.bottom
                anchors.left: parent.right
                anchors.leftMargin: scaleY(5)
                font.pixelSize: scaleY(3)
                text: filedetailsclass.album
                smooth: true
                color: "white"
                font.family: aeonSettings.name
            }
        }
        Text { // year
            y: scaleY(16.67) // 120
            font.pixelSize: scaleY(5.14) // 37
            //text: "2006"
            smooth: true
            color: "white"
            font.family: aeonSettings.name
        }
        Text { // genre
            x: scaleX(8.83) // 113
            y: scaleY(16.67) // 120
            font.pixelSize: scaleY(3.33) // 24
            text: filedetailsclass.genre
            smooth: true
            color: "white"
            font.family: aeonSettings.name
        }
        Text { // track
            y: scaleY(22.22) // 160
            font.pixelSize: scaleY(2.5) // 18
            //text: "Track:"
            smooth: true
            color: "white"
            font.family: aeonSettings.name
        }
        Text { // duration
            y: scaleY(26.39) // 190
            font.pixelSize: scaleY(2.5) // 18
            //text: "Duration:"
            smooth: true
            color: "white"
            font.family: aeonSettings.name
        }
        // Menu - possible flickable or other repeater
        Rectangle {
            x: scaleX(.23) // 3
            y: scaleY(44.7) //
            width: scaleX(49.22) // 630
            height: scaleY(4.03) // 29
            color: "transparent"
            clip: true
            Flickable{
                flickableDirection: Flickable.HorizontalFlick
                anchors.fill: parent
                Row{
                    anchors.fill: parent
                    Rectangle{
                        height: parent.height
                        width: parent.width/6
                        color: "transparent"
                        Text{
                            text: "Close"
                            smooth: true
                            color: "white"
                            font.family: aeonEdit.name
                            font.pixelSize: scaleY(2.92) // 21
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        MouseArea{
                            anchors.fill: parent
                            onClicked:
                                SequentialAnimation {
                                ScriptAction { script: stage.visible=true; }
                                PropertyAnimation { target: details; properties: "opacity"; to: 0; duration: 250 }
                                ScriptAction { script: details.destroy(); }
                            }

                        }
                    }
                    Rectangle{
                        height: parent.height
                        width: parent.width/6
                        color: "transparent"
                        Text{
                            text: "Play"
                            smooth: true
                            font.family: aeonEdit.name
                            font.pixelSize: scaleY(2.92) // 21
                            anchors.verticalCenter: parent.verticalCenter
                            color: "#868686"
                        }
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                manager.playMedia(filedetailsclass.file)  //dce function
                            }
                        }
                    }
                }
            }
        }
    }
    Component.onCompleted: details.opacity=1
}
