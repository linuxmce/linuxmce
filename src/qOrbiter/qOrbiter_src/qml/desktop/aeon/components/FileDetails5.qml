import QtQuick 1.1

// File info screen (movies/videos)
Item{
    id: details

    opacity: 0
    Behavior on opacity {
        PropertyAnimation{duration:350}
    }
    Component.onCompleted: {
        details.opacity=1
    }
    Connections{
        target: filedetailsclass
        onImageChanged: {
            coverArt.source = "image://listprovider/filedetailsprovider/"+securityvideo.timestamp
        }
        onSynopChanged:{
            synopsisflick.contentHeight=synopsistext.height
        }
        onPerformersChanged:{
            castList.model = filedetailsclass.performers
        }
    }

    MouseArea{
        anchors.fill: details
    }
    // Cover art
    Rectangle{
        anchors.right: rightInfoPanelBg.left
        anchors.left: parent.left
        height: parent.height
        color: "black"
        Image{
            id: coverArt
            height:parent.height
            fillMode: Image.PreserveAspectFit;
            source: ""
            smooth: true
        }
    }
    Image{
        id: rightInfoPanelBg
        source: "../img/info/lite/info_rightpaneltop.png"
        smooth: true
        height: parent.height
        width: scaleX(62.11) // 795
        x: scaleX(37.97) // 486
    }
    Rectangle{
        id: rightInfoPanel
        color: "transparent"
        x: scaleX(42.97) // 550
        y: scaleY(6.25) // 45
        width: scaleX(54.3) // 695
        height: scaleY(79.03) // 569
        Image{
            source: '../img/info/info_panellines_movie.png'
            smooth: true
            y: scaleY(8.333) // 60
            width: parent.width
            height: parent.height
        }


        // Static labels
        Text{
            x: scaleX(.23) // 3
            y: scaleY(17.22) // 113+11
            text: "Directed By"
            smooth: true
            color: "#868686"
            font.family: aeonEdit.name
            font.pixelSize: 16
        }
        Text{
            x: scaleX(.23) // 3
            y: scaleY(21.11) // 141+11
            text: "Written By"
            smooth: true
            color: "#868686"
            font.family: aeonEdit.name
            font.pixelSize: 16
        }
        Text{
            x: scaleX(.23) // 3
            y: scaleY(25) // 169+11
            text: "Duration"
            smooth: true
            color: "#868686"
            font.family: aeonEdit.name
            font.pixelSize: 16
        }
        Text{
            x: scaleX(.23) // 3
            y: scaleY(28.89) // 197+11
            text: "Starring"
            smooth: true
            color: "#868686"
            font.family: aeonEdit.name
            font.pixelSize: 16
        }

        // Title
        Text{
            text: filedetailsclass.objecttitle
            smooth: true
            color: "white"
            font.family: aeonSettings.name
            font.pixelSize: 50
        }

        // MPAA rating flag
        Image{
            //source: "../img/flags/mpaa_restricted.png" // as well as other mpaa_* files
            smooth: true
            x: scaleX(.55) // 7
            y: scaleY(9.44) // 68
            width: scaleX(12.97) // 166
            height: scaleY(5.28) // 38
        }
        // Year
        Rectangle{
            x: scaleX(14.14) // 181
            y: scaleY(8.06) // 58
            width: scaleX(8.52) // 109
            height: scaleY(7.78) // 56
            color:"transparent"
            Text{
                //text: "2006"
                smooth: true
                color: "#868686"
                font.family: aeonEdit.name
                font.pixelSize: 36
                anchors.centerIn: parent
            }
        }
        // Genre
        Rectangle{
            x: scaleX(24.22) // 310
            y: scaleY(8.06) // 58
            height: scaleY(7.78) // 56
            color:"transparent"
            Text{
                //text: "Action / Drama / History / War"
                smooth: true
                color: "#868686"
                font.family: aeonEdit.name
                font.pixelSize: 20
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        // Director
        Text{
            x: scaleX(9.38) // 120
            y: scaleY(17.22) // 113+11
            text: filedetailsclass.director
            smooth: true
            color: "white"
            font.family: aeonEdit.name
            font.pixelSize: 16
        }
        // Writer
        Text{
            x: scaleX(9.38) // 120
            y: scaleY(21.11) // 141+11
            text: filedetailsclass.composerlist
            smooth: true
            color: "white"
            font.family: aeonEdit.name
            font.pixelSize: 16
        }
        // Duration
        Text{
            x: scaleX(9.38) // 120
            y: scaleY(25) // 169+11
            //text: "120 minutes"
            smooth: true
            color: "white"
            font.family: aeonEdit.name
            font.pixelSize: 16
        }
        // Cast
        Rectangle{
            x: scaleX(9.38) // 120
            y: scaleY(28.2) // 203
            width: scaleX(39.06) // 500
            height: scaleY(15.28) // 110
            color: "transparent"
            clip: true
            ListView {
                id: castList
                anchors.fill: parent
                model: filedetailsclass.performers
                //                model:ListModel {
                //                    ListElement {
                //                        pkattribute: "douchebag one"
                //                    }
                //                    ListElement{
                //                        pkattribute: "douchebag two"
                //                    }
                //                }
                delegate: Item {
                    width: castList.width;
                    height: castList.height/5
                    Text {
                        text:modelData
                        smooth: true
                        color: "white"
                        font.family: aeonEdit.name
                        font.pixelSize: 16
                    }
                }
            }
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
                            font.pixelSize: 21
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
                            font.pixelSize: 21
                            anchors.verticalCenter: parent.verticalCenter
                            color: "#868686"
                        }
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                playMedia(filedetailsclass.file)  //dce function
                            }
                        }
                    }
                }
            }
        }
        // Synopsis
        Image{
            source: '../img/common/more_top.png'
            anchors.horizontalCenter: synopsisrect.horizontalCenter
            anchors.bottom: synopsisrect.top
            width: scaleX(4) // 27
            height: scaleY(1.94)
            smooth: true
            visible: !synopsisflick.atYBeginning
        }
        Rectangle {
            id: synopsisrect
            x: scaleX(.23) // 3
            y: scaleY(49.44) // 356
            width: scaleX(49.22) // 630
            height: scaleY(29)
            color: "transparent"
            clip: true
            Flickable{
                id: synopsisflick
                anchors.fill: parent
                flickableDirection: Flickable.VerticalFlick
                contentWidth: parent.width
                //contentHeight: childrenRect.height
                Text {
                    id: synopsistext
                    //anchors.fill: parent
                    width: parent.width
                    color: "white"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: filedetailsclass.synop
                    font.pixelSize: 21
                    font.family: aeonNowPlayingCE.name
                    smooth: true
                }
            }
        }
        Image{
            source: '../img/common/more_bottom.png'
            anchors.horizontalCenter: synopsisrect.horizontalCenter
            anchors.top: synopsisrect.bottom
            width: scaleX(4)
            height: scaleY(1.94)
            smooth: true
            visible: !synopsisflick.atYEnd
        }

        // IMDB rating
        Rectangle {
            x: scaleX(.23) // 3
            y: scaleY(81.97) // 583
            width: scaleX(8.59) // 110
            height: scaleY(4.03) // 29
            color: "transparent"
            Text{
                text: "IMDB"
                smooth: true
                color: "#868686"
                font.family: aeonEdit.name
                font.pixelSize: 25
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }

}
