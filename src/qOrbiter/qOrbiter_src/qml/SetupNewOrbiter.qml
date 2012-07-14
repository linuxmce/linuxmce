import QtQuick 1.0

Rectangle {
    width: appW
    height: appH
    color:"slategrey"

    function scaleX(x){
        return x/100*appH
    }
    function scaleY(y){
        return y/100*appW
    }

    Rectangle{
        id:titleBlock
        height: scaleY(15)
        width: scaleX(65)
        anchors.top: parent.top
        anchors.topMargin: scaleX(15)
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
            id: status
            text: qsTr("Setup New Orbiter")
            font.pixelSize: scaleY(4)
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    Row{
        id:orbiter_options
        spacing:5
        anchors.top: titleBlock.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        ListView{
            id:users_options
            height: scaleY(65)
            width: scaleX(10)
            //model:dummyModel
            delegate: Rectangle{
                height: scaleY(10)
                width: scaleX(9)
                color: "lightgrey"
                Text {
                    text: optionTitle
                }
            }
        }

        ListView{
            id:rooms_options
        }

        ListView{
            id:size_options
        }

        ListView{
            id:lang_options
        }

        ListView{
            id:skin_options
        }

    }

    ListModel{
        id:dummyModel
        ListElement{
            optionTitle:"foo"
            optionValue:1
        }
    }


}
