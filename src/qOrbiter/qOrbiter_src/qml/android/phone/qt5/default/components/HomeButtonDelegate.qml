import QtQuick 2.2

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

    Rectangle{
        anchors.fill: parent
        gradient: Gradient{
            GradientStop{position:0.0; color:  "transparent" }
            GradientStop{position: .65; color:"#40FFFFFF" }
        }

        border.color: androidSystem.orangeHighlight
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




