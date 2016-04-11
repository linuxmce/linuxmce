import QtQuick 1.1


Item{
    id:firstRunOptions
    state:"hidden"
    anchors.bottom: parent.bottom
    MouseArea{
        anchors.fill: parent
        onClicked: {}
    }

    Rectangle{
        id:phil
        anchors.fill: parent
        color:"grey"
        opacity: .65

        Behavior on opacity {
            PropertyAnimation{
                duration:350
            }
        }

        Behavior on height{
            PropertyAnimation{
                duration:350
            }
        }
    }

    Item{
        id:optionsRect
        anchors.fill: parent
        Text {
            id: optionLabel
            text: qsTr("Is this your first time connecting? Click here to set options.")
            anchors.centerIn: parent
            color:"white"
            font.weight: Font.Light
            font.pointSize: firstRunOptions.parent.height*.02
        }
        MouseArea{
            anchors.fill: parent
            onClicked: firstRunOptions.state = "editing"
        }
    }



    Flickable{
        id:selectionFlick
        width: optionCol.width

        anchors.centerIn: parent
        contentHeight: optionCol.height +( manager.appHeight*.65)
        Column{
            id:optionCol
            height: childrenRect.height
            width: manager.appWidth*.95
            spacing:manager.appHeight*.02

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            PropAndValDisplay {
                id:routerAddress
                title:"Home Ip / Host"
                value: manager.currentRouter
                onSetVal: {manager.currentRouter = editedValue}

            }
            PropAndValDisplay {
                id:routerPort
                title:"Web Port"
                value: manager.routerPort
                onSetVal: manager.routerPort = editedValue
            }
            PropAndValDisplay {
                id:awayIp
                title:"Away Ip / Host "
                value: manager.externalip
                onSetVal: manager.externalip = editedValue
            }

            PropAndValDisplay {
                id:skin
                title:"Skin"
                value: manager.currentSkin
                onSetVal: manager.currentSkin = editedValue
            }

            PropAndValDisplay {
                id:deviceType
                useText: true
                title:"Type of Device"
                value: manager.isPhone
                onSetVal: manager.isPhone = editedValue
            }
        }
    }

    Item {
        id:closeBtn
        height: 50
        width: 120
        anchors.top: parent.top
        anchors.right: parent.right

        Rectangle{
            anchors.fill: parent
            radius:10
            color:"darkgreen"
            border.color: "white"
            border.width: 2

        }

        Text{
            text:"Close"
            anchors.centerIn: parent
            font.pointSize: 16
        }

        MouseArea{
            anchors.fill: parent
            onClicked: {
                manager.writeConfig()
                firstRunOptions.state = "hidden"
                rootItem.forceActiveFocus()
                //  manager.restartFomUi()
            }
        }
    }


    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: firstRunOptions
                height: manager.appHeight*.10
                width:parent.width
            }
            PropertyChanges {
                target: phil
                opacity:.65
            }
            PropertyChanges {
                target: closeBtn
                visible:false
            }
            PropertyChanges{
                target:optionsRect
                visible:true
            }
            PropertyChanges{
                target:optionCol
                visible:false
            }
            PropertyChanges{
                target:selectionFlick
                height:0
            }
        },
        State {
            name: "editing"
            PropertyChanges {
                target: firstRunOptions
                height:manager.appHeight
                width:manager.appWidth
            }
            PropertyChanges{
                target:phil
                opacity:1
            }
            PropertyChanges {
                target: closeBtn
                visible:true
            }
            PropertyChanges{
                target:optionsRect
                visible:false
            }
            PropertyChanges {
                target: selectionFlick
                height: parent.height
            }
            PropertyChanges{
                target:optionCol
                visible:true
            }
        }
    ]
}
