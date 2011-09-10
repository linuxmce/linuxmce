import QtQuick 1.0


Rectangle {
    id: filedetailrect
    width: scaleX(50)
    height: scaleY(80)
    anchors.centerIn: parent
    color: style.darkhighlight
    radius: 20

    Timer{
        id:singleshot
        repeat: false
        interval: 250
        triggeredOnStart: false
        onTriggered: image1.source = "image://filedetailsprovider/"+securityvideo.timestamp
        running: true
    }

    clip: true
    focus: true
    Rectangle{
        id:titlerect
        height: childrenRect.height + 5
        width: parent.width
        color:style.lighthighlight

        Text {
            id: text2
            x: 225
            y: 5
            text: filedetailsclass.objecttitle
            font.pixelSize: 14
            font.bold: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
    }

    Image {
        id: image1
        anchors.top: titlerect.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: scaleX(60)
        height: scaleY(30)
        fillMode: Image.PreserveAspectFit
        source: "image://filedetailsprovider/"+filedetailsclass.screenshot
        asynchronous: true
    }

    Rectangle {
        id: rectangle1
       anchors.top: image1.bottom
        width: scaleX(60)
        height: scaleY(20)
        radius: 4
        clip:  true
        color: style.not_color

        Flickable {
            anchors.fill: parent
            flickableDirection: Flickable.VerticalFlick
            contentWidth: parent.width
            contentHeight: childrenRect.height
            id: contentFlick
            Column
            {
                spacing:5


            Text {
                id: fnametext               
                text: "Filename: " + filedetailsclass.filename
                font.pixelSize: 12
            }

            Text {
                id:  titletext
                text:"Title:" + filedetailsclass.objecttitle
                font.pixelSize: 12
            }

            Text {
                id: synopsistext

                width: scaleX(50)
                wrapMode: "WrapAtWordBoundaryOrAnywhere"
                text: filedetailsclass.synop
                font.pixelSize: 12
            }
        }
    }

    Text {
        id: txtmoredn        
        text: "More..."
        anchors.horizontalCenterOffset: 294
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 12
        visible: !contentFlick.atYEnd
    }

    Text {
        id: txtmoreup
        x: scaleX(60)
        y: scaleY(20)
        text: "More..."
        anchors.horizontalCenterOffset: 294
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 12
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
anchors.top: rectangle1.bottom
ButtonSq {
    id: buttonsq1

    width: style.stdbuttonw
    height: style.stdbuttonh
    radius: 10

    //anchors.leftMargin: 18
    buttontext: "Play"
    MouseArea
    {
        anchors.fill: parent
        onClicked: playMedia(filedetailsclass.file)  //dce function
    }
}

ButtonSq {
    id: buttonsq2

    width: style.stdbuttonw
    height: style.stdbuttonh
    radius: 10

    buttontext: "Move"
}

ButtonSq {
    id: buttonsq3

    width: style.stdbuttonw
    height: style.stdbuttonh
    radius: 10
    buttontext: "Close"
    x: ((parent.width/3)*2)
    MouseArea{
        anchors.fill:  parent
        onClicked: filedetailrect.destroy()
    }
}
}
Component.onCompleted: contentFlick.contentHeight=synopsistext.height+104

}






