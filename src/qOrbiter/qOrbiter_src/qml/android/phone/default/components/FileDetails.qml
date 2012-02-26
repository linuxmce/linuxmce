import QtQuick 1.0


Rectangle {
    id: filedetailrect
    width: scaleX(85)
    height: scaleY(85)
    anchors.centerIn: parent
    color: "silver"
    clip: true
    radius: 5
    border.color:"orange"
    border.width: 1
    MouseArea{
        id:bg_catch
        height: appH
        width: appW
        onPressed: filedetailrect.destroy()

    }

    Image {
        id: fdbg
        source: "../img/bkg.png"
        anchors.fill: filedetailrect
    }
    Timer{
        id:singleshot
        repeat: false
        interval: 250
        triggeredOnStart: false
        onTriggered: filedetailsimage.source = "image://listprovider/filedetailsprovider/"+securityvideo.timestamp
        running: true
    }


    Rectangle{
        id:imageholder
        height:childrenRect.height
        width:childrenRect.width
        color: "transparent"
        anchors.verticalCenter: parent.verticalCenter

        anchors.left: parent.left
        anchors.leftMargin: filedetailsclass.aspect=="wide"? scaleX(2.6) : scaleX(15)
        BorderImage {
            id: borderimg
            horizontalTileMode: BorderImage.Repeat
            source: "../img/drpshadow.png"
            anchors.fill: filedetailsimage
            anchors { leftMargin: -6; topMargin: -6; rightMargin: -8; bottomMargin: -8 }
            border { left: 10; top: 10; right: 10; bottom: 10 }
            smooth: true
        }
        Image {
            id: filedetailsimage
            width: scaleX(25)
            height:scaleY(25)
            source: "image://listprovider/filedetails/"+securityvideo.timestamp
            smooth: true
        }

        Image {
            id: npmask
            source: "../img/nowplayingbox.png"
            anchors.fill: filedetailsimage
            opacity: .5

        }
    }


    Rectangle {
        id: rectangle1
        anchors.verticalCenter: imageholder.verticalCenter
        width:  filedetailsclass.aspect=="wide"? parent.width *.40 : parent.width *.45
        height: scaleY(35)
        radius: 2.5
        clip:  true
        color: "black"
        border.color: "orange"
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
                Text {
                    id: fnametext

                    font.pixelSize: scaleY(2)
                    color:"aliceblue"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                     width: rectangle1.width *.95
                }

                Text {
                    id:  titletext
                    text:filedetailsclass.mediatitle
                    font.pixelSize: scaleY(3)
                    font.bold: true
                    color:"aliceblue"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                     width: rectangle1.width *.95
                }
                Text {
                    id:  episode
                    text:filedetailsclass.episode
                    font.pixelSize: scaleY(2)
                    font.bold: true
                    color:"aliceblue"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                     width: rectangle1.width *.95
                }

                Text {
                    id: rating
                   width: rectangle1.width *.95
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Rating")
                    font.family: "Droid Sans"
                    //  font.bold: true
                    smooth: true
                    font.pixelSize: scaleY(2)
                     color:"aliceblue"
                    elide: "ElideRight"
                    visible:  filedetailsclass.rating ==="" ? false: true
                }

                Text {
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


                Text {
                    id: synopsistext
                    width: rectangle1.width *.95
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: filedetailsclass.synop
                    font.pixelSize: scaleY(2)
                    color:"aliceblue"
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
            color: style.button_system_color

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
                onClicked:{ playMedia(filedetailsclass.file) ; filedetailrect.destroy()}

            }
        }

        AvOptionButton {
            id: buttonsq2
            width: style.stdbuttonw
            height: style.stdbuttonh
            radius: 10
            buttontext: "Move"
        }

        AvOptionButton {
            id: buttonsq3
            height: style.stdbuttonh
            width: style.stdbuttonw
            radius: 10
            buttontext: "Close"
            x: ((parent.width/3)*2)
            MouseArea{
                anchors.fill:  parent
                onClicked: { dataModel.checkForMore();filedetailrect.destroy()}
            }
        }
    }
    Component.onCompleted: contentFlick.contentHeight=synopsistext.height+104

}






