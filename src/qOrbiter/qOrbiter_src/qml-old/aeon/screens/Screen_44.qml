import QtQuick 1.1
import "../components"
import "../js/ComponentLoader.js" as MyJs
Item {
    id:screen_44
    height: skinStyle.orbiterH
    width: skinStyle.orbiterW
    Rectangle {
        height: skinStyle.orbiterH
        width: skinStyle.orbiterW
        color:  "black"
        opacity: .8
    }

Rectangle {
    height: skinStyle.orbiterH
    width: skinStyle.orbiterW
    color: "transparent"
    id: advancedscreen

    Rectangle {
         border.width: 1
         border.color: skinStyle.button_action_color_hover
        id:home
        height: skinStyle.buttonH
        width: skinStyle.buttonW
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
             border.color: skinStyle.button_action_color_hover
            id:regenorbiter
            height: skinStyle.buttonH
            width: skinStyle.buttonW
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
            border.color: skinStyle.button_action_color_hover
            id:pending
            height: skinStyle.buttonH
            width: skinStyle.buttonW
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
            border.color: skinStyle.button_action_color_hover
            id:networksettings
            height: skinStyle.buttonH
            width: skinStyle.buttonW
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
            border.color: skinStyle.button_action_color_hover
            id:avwizard
            height: skinStyle.buttonH
            width: skinStyle.buttonW
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
            height: skinStyle.buttonH
            width: skinStyle.buttonW
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
            height: skinStyle.buttonH
            width: skinStyle.buttonW
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
            id:changeskinStyle
            height: skinStyle.buttonH
            width: skinStyle.buttonW
            radius: 5
            Text {
                text:"Change skinStyles"
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
                onClicked: MyJs.createStageComponent("SkinSelector.qml", screen_44 )
            }
        }



    }

}
}
