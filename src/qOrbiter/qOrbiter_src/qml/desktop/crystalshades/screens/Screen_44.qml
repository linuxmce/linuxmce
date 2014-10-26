import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs
Item {
    anchors.fill: parent
    //height: style.orbiterH *.7 //*.7 by RayBe
    //width: style.orbiterW *.7 //*.7 by RayBe
/*
    Rectangle {
        height: style.orbiterH *.7 //*.7 by RayBe
        width: style.orbiterW *.7 //*.7 by RayBe
        color:  "black"
        opacity: .8
    }
*/
Rectangle {
    anchors.fill: parent
    //height: style.orbiterH *.7 //*.7 by RayBe
    //width: style.orbiterW *.7 //*.7 by RayBe
    color: "darkgrey"
    id: advancedscreen

    Rectangle {
         border.width: 1
         border.color: style.button_action_color_hover

        id:home
        height: style.buttonH
        width: style.buttonW
        radius: 5
        Text {
            text:"Home"
            font.pointSize: 10
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            anchors.fill: parent
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 5
            font.family: "Droid Sans"
            wrapMode: Text.WordWrap

             }

        MouseArea{
            anchors.fill:parent
            z:5
            onClicked: setCurrentScreen("Screen_1.qml")
        }
    }

    Flow {
        id: flow1
        anchors.centerIn: parent
        height: 400
        width: 400
        spacing: 10

        Rectangle {
             border.width: 1
             border.color: style.button_action_color_hover
            id:regenorbiter
            height: style.buttonH
            width: style.buttonW
            radius: 5
            Text {
                text:"Regen Orbiter"
                font.pointSize: 10
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 5
                font.family: "Droid Sans"
                wrapMode: Text.WordWrap

                 }

            MouseArea{
                anchors.fill:parent
                z:5
                onClicked: regenOrbiter(35)
            }
        }

        Rectangle {
            border.width: 1
            border.color: style.button_action_color_hover
            id:pending
            height: style.buttonH
            width: style.buttonW
            radius: 5
            Text {
                text:"Pending Tasks"
                font.pointSize: 10
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 5
                font.family: "Droid Sans"
                wrapMode: Text.WordWrap

                 }
        }

        Rectangle {
            border.width: 1
            border.color: style.button_action_color_hover
            id:networksettings
            height: style.buttonH
            width: style.buttonW
            radius: 5
            Text {
                text:"Network Settings"
                font.pointSize: 10
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 5
                font.family: "Droid Sans"
                wrapMode: Text.WordWrap

                 }
        }

        Rectangle {
            border.width: 1
            border.color: style.button_action_color_hover
            id:avwizard
            height: style.buttonH
            width: style.buttonW
            radius: 5
            Text {
                text:"Av wizard"
                font.pointSize: 10
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 5
                font.family: "Droid Sans"
                wrapMode: Text.WordWrap

                 }
        }

        Rectangle {
            id:regenorbiters
            height: style.buttonH
            width: style.buttonW
            radius: 5
            Text {
                text:"Regen All Orbiters"
                font.pointSize: 10
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 5
                font.family: "Droid Sans"
                wrapMode: Text.WordWrap

                 }
        }

        Rectangle {
            id:quickreload
            height: style.buttonH
            width: style.buttonW
            radius: 5
            Text {
                text:"Quick Reload"
                font.pointSize: 10
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 5
                font.family: "Droid Sans"
                wrapMode: Text.WordWrap

                 }
            MouseArea{
                anchors.fill: parent
                z:10
                onClicked: quickReload()
            }
        }

        Rectangle {
            id:changeStyle
            height: style.buttonH
            width: style.buttonW
            radius: 5
            Text {
                text:"Change Styles"
                font.pointSize: 10
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                anchors.fill: parent
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 5
                font.family: "Droid Sans"
                wrapMode: Text.WordWrap

                 }

            MouseArea{
                anchors.fill: parent
                onClicked: MyJs.createStageComponent("SkinSelector.qml", advancedscreen)
            }
        }



    }

}
}
