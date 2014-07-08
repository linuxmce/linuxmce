import QtQuick 1.0

Item {
    id: filedetailrect
    width: scaleX(100)
    height: scaleY(100)
    anchors.centerIn: parent
    property int bgImageProp:manager.q_subType ==="1" ? 43 : manager.q_attributetype_sort===53 ? 43 :36
    clip: true
    scale:0
    Component.onCompleted: PropertyAnimation { target: filedetailrect; property: "scale"; to:1; duration: 500}

    MouseArea{
        anchors.fill: parent
        onClicked: {}
    }

    Image{
        id:imdb
        fillMode: Image.PreserveAspectCrop
        anchors.fill: parent
        source:"http://"+m_ipAddress+"/lmce-admin/imdbImage.php?type=atr&file="+filedetailsclass.file+"&val="+bgImageProp
        // onStatusChanged: imdb.status == Image.Ready ? filedetailrect.height = scaleY(100) : ""
    }

    Rectangle{
        color: "black"
        border.color: "orange"
        border.width: 3
        anchors.fill: parent
        opacity: .55
    }

    Rectangle{
        id:titlerect
        height: childrenRect.height + 5
        width: parent.width
        color:"black"
        radius:2.5
        anchors.top:parent.top
        anchors.horizontalCenter: parent.horizontalCenter
//        Text {
//            id: text2
//            anchors.centerIn: parent
//            text: qsTr("File Location: ") + filedetailsclass.path
//            font.pixelSize: scaleY(2)
//            font.bold: true
//            color:"white"
//            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
//        }
        Text {
            id: filename_block
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Filename: " + filedetailsclass.file
            font.pixelSize: scaleY(3)
            font.bold: true
            color:"white"
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            anchors.bottom: parent.bottom

        }
    }


    Item {
        id: displayData
        anchors.centerIn: parent
        width:  parent.width*.95
        height: scaleY(75)
        clip:  true

        Flickable {
            anchors.fill: parent
            flickableDirection: Flickable.VerticalFlick
            contentWidth: textcol.width
            contentHeight: textcol.height
            id: contentFlick
            Column
            {
                id:textcol
                spacing:scaleY(2)
                anchors.margins: scaleY(1)
                width: displayData.width
                height: childrenRect.height
                Text {
                    id:  titletext
                    text:filedetailsclass.mediatitle
                    font.pixelSize: scaleY(4)
                    font.bold: true
                    color:"aliceblue"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    width:scaleX(75)
                }

                Text {
                    id:  program_block
                    text:qsTr("Program") + filedetailsclass.program
                    font.pixelSize: scaleY(2)
                    font.bold: true
                    color:"aliceblue"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    visible:  filedetailsclass.program =="" ? false: true
                }

                Text {
                    id:  episode
                    text:qsTr("Episode") + filedetailsclass.episode
                    font.pixelSize: scaleY(2)
                    font.bold: true
                    color:"aliceblue"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    visible:  filedetailsclass.episode =="" ? false: true
                }
                Text {
                    id: album_block
                    width: scaleX(35)
                    text: qsTr("Album: ") + filedetailsclass.album
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    smooth: true
                    color:"white"
                    font.pixelSize: scaleY(2)
                    visible:  filedetailsclass.album =="" ? false: true
                }

                  Text {
                    id: rating

                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Rating") + filedetailsclass
                    font.family: "Droid Sans"
                    font.bold: false
                    smooth: true
                    font.pixelSize: scaleY(2)
                     color:"aliceblue"
                    elide: "ElideRight"
                    visible:  filedetailsclass.rating =="" ? false: true
                }

                Text {
                    id: genre_block
                    width: parent.width
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: filedetailsclass.genre
                    font.bold: false
                    smooth: true
                    font.pixelSize: scaleY(3)
                    color:"aliceblue"
                    elide: "ElideRight"
                    visible:  filedetailsclass.genre =="" ? false: true

                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: { starring.elide = "ElideNone" ; }
                        onExited: {starring.elide = "ElideRight"; }
                    }
                }
                Text {
                    id: director_block
                    width: parent.width
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Directed By: ") + filedetailsclass.director
                    font.bold: false
                    smooth: true
                    font.pixelSize: scaleY(3)
                    color:"aliceblue"
                    elide: "ElideRight"
                    visible:  filedetailsclass.director =="" ? false: true
                    MouseArea{
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: { starring.elide = "ElideNone" ; }
                        onExited: {starring.elide = "ElideRight"; }
                    }
                }

                Text {
                    id: starring
                    width: parent.width
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Starring: ") + filedetailsclass.performerlist
                    //  font.bold: true
                    smooth: true
                    font.pixelSize: scaleY(3)
                    color:"aliceblue"
                    elide: "ElideRight"
                    visible:  filedetailsclass.performerlist =="" ? false: true

                    MouseArea{
                        anchors.fill: starring
                        hoverEnabled: true
                        onEntered: { starring.elide = "ElideNone" ; }
                        onExited: {starring.elide = "ElideRight"; }
                    }
                }

                Text {
                    id: synopsistext
                    width: parent.width
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: filedetailsclass.synop
                    font.pixelSize: scaleY(3)
                    color:"white"
                }
            }
        }

        Text {
            id: txtmoredn
            text: "More..."
            anchors.horizontalCenterOffset: 294
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: scaleY(2)
            color:"aliceblue"
            visible: !contentFlick.atYEnd
        }

        Text {
            id: txtmoreup
            x: scaleX(60)
            y: scaleY(20)
            text: "More..."
            anchors.horizontalCenterOffset: 294
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: scaleY(2)
            color:"aliceblue"
            visible: !contentFlick.atYBeginning
        }
    }

    Component{
        id:performersheader

        Rectangle {
            width: container.width
            height: childrenRect.height
            color: appStyle.button_system_color

            Text {
                text: section
                font.bold: true
            }
        }

    }

    /* ListView {
        id: list_view1
        x: 380
        y: 16
        width: 256
        height: 154
        model: filedetails.performers

        delegate: Item {
            width: list_view1.width;
            Text {
            id: titleof
            text:pkattribute
            height:  20
            }
        }

    } */

    Row{
        id:controlrow
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: filedetailsclass.aspect=="wide"? parent.horizontalCenter : rectangle1.horizontalCenter
        spacing: scaleY(.5)
        AvOptionButton {
            id: buttonsq1
            width: appStyle.stdbuttonw
            height: appStyle.stdbuttonh
            radius: 10

            //anchors.leftMargin: 18
            buttontext: "Play"
            MouseArea
            {
                anchors.fill: parent
                onClicked:{ manager.playMedia(filedetailsclass.file); loadComponent("NullComponent.qml") }  //dce function
            }
        }

        AvOptionButton {
            id: buttonsq2
            width: appStyle.stdbuttonw
            height: appStyle.stdbuttonh
            radius: 10
            buttontext: "Move"
        }

        AvOptionButton {
            id: buttonsq3
            width: appStyle.stdbuttonw
            height: appStyle.stdbuttonh
            radius: 10
            buttontext: "Close"
            x: ((parent.width/3)*2)
            MouseArea{
                anchors.fill:  parent
                onClicked: { dataModel.setLoadingStatus(true);filedetailsclass.clear(); manager.setGridStatus(true); loadComponent("NullComponent.qml")}
            }
        }
    }


}






