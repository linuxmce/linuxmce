import QtQuick 2.0
//import Qt.labs.shaders 1.0
import "../effects"

Rectangle {
    id: filedetailrect
    width: scaleX(80)
    height: scaleY(85)
  anchors.centerIn: parent
    color: style.highlight2
    clip: false
    radius: 5
    border.color: style.highlight1
    border.width: 3
    //opacity: 0
    scale:0
    Component.onCompleted: PropertyAnimation { target: filedetailrect; property: "scale"; to:1; duration: 500}

    Image {
        id: fdbg
        source: "../img/icons/nowplaying.png"
        anchors.fill: filedetailrect
    }

    Rectangle{
        id:masking_rect
        height: manager.appHeight
        width: manager.appWidth
        color: "orange"
        opacity: .75
        z:-1
        anchors.centerIn: parent
        MouseArea{
            anchors.fill: parent
            onClicked: loadComponent("NullComponent")
        }
    }


    Connections{
        target:filedetailsclass
        onImageChanged:filedetailsimage.source = "image://listprovider/filedetailsprovider/"+securityvideo.timestamp
    }

    Rectangle{
        id:titlerect
        height: childrenRect.height + 5
        width: parent.width
        color:style.highlight1
        radius:2.5
        StyledText {
            id: text2
            anchors.horizontalCenter: parent.horizontalCenter
               text: "Location: " + filedetailsclass.path
               font.pixelSize: scaleY(2)
            font.bold: true
            wrapMode: StyledText.WrapAtWordBoundaryOrAnywhere
        }

    }
    StyledText {
        id: filename_block
        anchors.horizontalCenter: parent.horizontalCenter
           text: "Filename: " + filedetailsclass.file
           font.pixelSize: scaleY(3)
        font.bold: true
        wrapMode: StyledText.WrapAtWordBoundaryOrAnywhere
        anchors.bottom: parent.bottom

    }

    Rectangle{
        id:imageholder
        height:childrenRect.height
        width:childrenRect.width
        color: "transparent"
        anchors.verticalCenter: parent.verticalCenter

        anchors.left: parent.left
        anchors.leftMargin: filedetailsclass.aspect=="wide"? scaleX(2.6) : scaleX(7)
        BorderImage {
            id: borderimg
            horizontalTileMode: BorderImage.Repeat
            source: "../img/icons/drpshadow.png"
            anchors.fill: filedetailsimage
            anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
            border { left: 10; top: 10; right: 10; bottom: 10 }
            smooth: true
        }
        Image {
            id: filedetailsimage
            width: filedetailsclass.aspect=="wide"? scaleX(42.5) : scaleX(23)
            height:filedetailsclass.aspect=="wide"?scaleY(42.5) : scaleY(55)
            source: "../img/icons/mediatime.png"
            smooth: true
        }

        Image {
            id: npmask
            source: "../img/icons/transparencymask.png"
            anchors.fill: filedetailsimage
            opacity: .5
        }
    }


    Rectangle {
        id: rectangle1
        anchors.verticalCenter: imageholder.verticalCenter
        width:  filedetailsclass.aspect=="wide"? parent.width *.40 : parent.width *.45
        height: scaleY(45)
        radius: 2.5
        clip:  true
        color: style.darkhighlight
        border.color: style.highlight1
        anchors.left: imageholder.right
        anchors.leftMargin: scaleX(.5)

        Flickable {
            anchors.fill: parent
            flickableDirection: Flickable.VerticalFlick
            contentWidth: parent.width
            contentHeight: textcol.height
            id: contentFlick
            Column
            {
                id:textcol
                spacing:5
                anchors.margins: scaleY(1)
                width: parent.width
                height: childrenRect.height
                StyledText {
                    id: fnametext

                    font.pixelSize: scaleY(2)
                    color:"aliceblue"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                     width: rectangle1.width *.95
                     font.bold: true
                }

                StyledText {
                    id:  titletext
                    text:filedetailsclass.qs_mainTitle
                    font.pixelSize: scaleY(3)
                    font.bold: true
                    color:"aliceblue"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                     width: rectangle1.width *.95
                }

                StyledText {
                    id:  program_block
                    text:qsTr("Program") + filedetailsclass.program
                    font.pixelSize: scaleY(2)
                    font.bold: true
                    color:"aliceblue"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                     width: rectangle1.width *.95
                     visible:  filedetailsclass.program =="" ? false: true
                }

                StyledText {
                    id:  episode
                    text:qsTr("Episode") + filedetailsclass.episode
                    font.pixelSize: scaleY(2)
                    font.bold: true
                    color:"aliceblue"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                     width: rectangle1.width *.95
                     visible:  filedetailsclass.episode =="" ? false: true
                }
                StyledText {
                    id: album_block
                    width: scaleX(35)
                    text: qsTr("Album: ") + filedetailsclass.album
                    font.family: "Droid Sans"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    smooth: true
                    font.pixelSize: scaleY(2)
                    visible:  filedetailsclass.album =="" ? false: true
                }

              /*  StyledText {
                    id: rating
                   width: rectangle1.width *.95
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Rating") + filedetailsclass
                    font.family: "Droid Sans"
                    //  font.bold: true
                    smooth: true
                    font.pixelSize: scaleY(2)
                     color:"aliceblue"
                    elide: "ElideRight"
                    visible:  filedetailsclass.rating =="" ? false: true
                }
                */
                StyledText {
                    id: genre_block
                   width: rectangle1.width *.95
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: filedetailsclass.genre
                    font.family: "Droid Sans"
                    //  font.bold: true
                    smooth: true
                    font.pixelSize: scaleY(2)
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
                StyledText {
                    id: director_block
                   width: rectangle1.width *.95
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Directed By: ") + filedetailsclass.director
                    font.family: "Droid Sans"
                    //  font.bold: true
                    smooth: true
                    font.pixelSize: scaleY(2)
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

                StyledText {
                    id: starring
                   width: rectangle1.width *.95
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Starring: ") + filedetailsclass.performerlist
                    font.family: "Droid Sans"
                    //  font.bold: true
                    smooth: true
                    font.pixelSize: scaleY(2)
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

                StyledText {
                    id: synopsistext
                    width: rectangle1.width *.95
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: filedetailsclass.synop
                    font.pixelSize: scaleY(2)
                    color:"aliceblue"
                }
            }
        }

        StyledText {
            id: txtmoredn
            text: "More..."
            anchors.horizontalCenterOffset: 294
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: scaleY(2)
            color:"aliceblue"
            visible: !contentFlick.atYEnd
        }

        StyledText {
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
            color: style.button_system_color

            StyledText {
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
            StyledText {
            id: titleof
            text:pkattribute
            height:  20
            }
        }

    } */

    Row{
        id:controlrow
        anchors.top:filedetailsclass.aspect=="wide"? imageholder.bottom: rectangle1.bottom
        anchors.topMargin: scaleY(1)
        anchors.horizontalCenter: filedetailsclass.aspect=="wide"? parent.horizontalCenter : rectangle1.horizontalCenter
        spacing: scaleY(.5)
        AvOptionButton {
            id: buttonsq1
            width: style.stdbuttonw
            height: style.stdbuttonh
            radius: 10

            //anchors.leftMargin: 18
            buttontext: "Play"
            MouseArea
            {
                anchors.fill: parent
                onClicked:{manager.playMedia(filedetailsclass.file); filedetailrect.destroy() }  //dce function
            }
        }

        AvOptionButton {
            id: buttonsq2
            width: style.stdbuttonw
            height: style.stdbuttonh
            radius: 10
            buttontext: "Move"
            MouseArea{
                anchors.fill:  parent
                onClicked: {  filedetailrect.destroy()}
            }
        }

        AvOptionButton {
            id: buttonsq3
            width: style.stdbuttonw
            height: style.stdbuttonh
            radius: 10
            buttontext: "Close"
            x: ((parent.width/3)*2)
            MouseArea{
                anchors.fill:  parent
                onClicked: {  filedetailsclass.clear(); manager.setGridStatus(true); filedetailrect.destroy()}
            }
        }
    }


}






