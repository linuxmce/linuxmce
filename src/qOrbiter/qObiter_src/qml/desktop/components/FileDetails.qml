import QtQuick 1.0

Rectangle {
    width: 650
    height: 550
    anchors.centerIn: parent

    Image {
        id: image1
        x: 70
        y: 13
        width: 495
        height: 293
        source: "../../../img/lmcesplash.jpg"
    }

    ListView {
        id: list_view1
        x: 70
        y: 354
        width: 495
        height: 170
        model: filedetails
        delegate: Rectangle{
            width: 495
            height: 50
            Row{
                width: parent.width
                height: parent.height

                Text {
                    id: titleof
                    text:id
                }

                Text {
                    id: attribname
                    text: title
                }
            }




        }
    }

}
