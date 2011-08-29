import QtQuick 1.0

Rectangle {
    width: 200
    height: 500
    color: "transparent"

    ListView{
        height:500
        width: 200

        clip: true
        interactive: true
        flickableDirection: "VerticalFlick"
        model: mediaplaylist

        delegate:

            Rectangle {
            width:200
            height: 50
            color: "whitesmoke"
            Text {
                text: id + " pos:" + name
            }
        }
    }
}
