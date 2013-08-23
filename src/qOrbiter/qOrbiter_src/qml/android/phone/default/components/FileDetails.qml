import QtQuick 1.0

Item {
    id: filedetailrect
    width: scaleX(100)
    height: scaleY(100)
    anchors.centerIn: parent
    Rectangle{
         color: "darkgrey"
        border.color: "orange"
        border.width: 3
        anchors.fill: parent
        MouseArea{
            anchors.fill: parent
            onClicked: loadCompnent("NullComponent.qml")
        }
    }

    clip: true
    //opacity: 0
    scale:0
    Component.onCompleted: PropertyAnimation { target: filedetailrect; property: "scale"; to:1; duration: 1000}

    Image {
        id: fdbg
        source: "../img/bkg.png"
        anchors.fill: filedetailrect
    }

    Connections{
        target:filedetailsclass
        onObjectChanged:filedetailsimage.source = "image://listprovider/filedetailsprovider/"+securityvideo.timestamp
    }

    Rectangle{
        id:titlerect
        height: childrenRect.height + 5
        width: parent.width
        color:"black"
        radius:2.5
        Text {
            id: text2
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Location: " + filedetailsclass.path
            font.pixelSize: scaleY(2)
            font.bold: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

    }
    Text {
        id: filename_block
        anchors.horizontalCenter: parent.horizontalCenter
        text: "Filename: " + filedetailsclass.file
        font.pixelSize: scaleY(3)
        font.bold: true
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        anchors.bottom: parent.bottom

    }


    Rectangle {
        id: rectangle1
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        width:  filedetailsclass.aspect=="wide"? parent.width *.40 : parent.width *.45
        height: scaleY(75)
        radius: 2.5
        clip:  true
        color: "black"
        border.color: "orange"
        anchors.left: imageholder.right
        anchors.leftMargin: scaleX(-10)
        smooth:true
        transform: Rotation { origin.x: 30; origin.y: 30; axis { x: 0; y: 1; z: 0 } angle: -25 }


        Flickable {
            anchors.fill: parent
            flickableDirection: Flickable.VerticalFlick
            contentWidth: parent.width
            contentHeight: textcol.height
            id: contentFlick
            Column
            {
                id:textcol
                spacing:scaleY(2)
                anchors.margins: scaleY(1)
                width: parent.width
                height: childrenRect.height
                Text {
                    id:  titletext
                    text:filedetailsclass.mediatitle
                    font.pixelSize: scaleY(3)
                    font.bold: true
                    color:"aliceblue"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    width: parent.width

                }
                Text {
                    id: fnametext

                    font.pixelSize: scaleY(2)
                    color:"aliceblue"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    width: rectangle1.width *.95
                }



                Text {
                    id:  program_block
                    text:qsTr("Program") + filedetailsclass.program
                    font.pixelSize: scaleY(2)
                    font.bold: true
                    color:"aliceblue"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    width: rectangle1.width *.95
                    visible:  filedetailsclass.program =="" ? false: true
                }

                Text {
                    id:  episode
                    text:qsTr("Episode") + filedetailsclass.episode
                    font.pixelSize: scaleY(2)
                    font.bold: true
                    color:"aliceblue"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    width: rectangle1.width *.95
                    visible:  filedetailsclass.episode =="" ? false: true
                }
                Text {
                    id: album_block
                    width: scaleX(35)
                    text: qsTr("Album: ") + filedetailsclass.album
                    font.family: "Droid Sans"
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    smooth: true
                    font.pixelSize: scaleY(2)
                    visible:  filedetailsclass.album =="" ? false: true
                }

                /*  Text {
                    id: rating
                   width: rectangle1.width *.95
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
                */
                Text {
                    id: genre_block
                    width: rectangle1.width *.95
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: filedetailsclass.genre
                    font.family: "Droid Sans"
                    font.bold: false
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
                Text {
                    id: director_block
                    width: rectangle1.width *.95
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    text: qsTr("Directed By: ") + filedetailsclass.director
                    font.family: "Droid Sans"
                    font.bold: false
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
        anchors.bottom: parent.bottom
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
                onClicked:{ manager.playMedia(filedetailsclass.file); loadComponent("NullComponent.qml") }  //dce function
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
            width: style.stdbuttonw
            height: style.stdbuttonh
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






