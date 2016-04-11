import QtQuick 1.0

Item{
    id:btnDelegate
    width: scaleX(61)
    height: scaleY(12)

    Rectangle{
        anchors.fill: parent
        color:scenario_handler.pressed ? androidSystem.orangeStandard : "black"
        Behavior on color{
            PropertyAnimation{
                duration:350
            }
        }
    }

    StyledText{
        text:title
        color:"orange"
        fontSize: scaleY(4)
        anchors.centerIn: parent
    }


    MouseArea{
        id:scenario_handler
        anchors.fill: parent
        onClicked: {
            manager.execGrp(params)
            currentModel = dummy
        }
    }
}




