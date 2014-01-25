import QtQuick 2.2


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
        color:"black"
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
            font.pointSize: scaleY(2)
        }
        MouseArea{
            anchors.fill: parent
            onClicked: firstRunOptions.state = "editing"
        }
    }

    Item {
        id:closeBtn
        height: scaleY(10)
        width: scaleX(10)
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
            font.pointSize: 18
        }

        MouseArea{
            anchors.fill: parent
            onClicked: {
                manager.writeConfig()
                firstRunOptions.state = "hidden"
            }
        }
    }

    Column{
        id:optionCol
        height: childrenRect.height
        width: parent.width*.75
        spacing:scaleY(1)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        PropAndValDisplay {
            id:routerAddress
            title:"Home Router Address"
            value: manager.m_ipAddress
            onSet: {manager.m_ipAddress = editedValue}
        }
        PropAndValDisplay {
            id:routerPort
            title:"Home Router Port"
            value: manager.routerPort
            onSet: manager.routerPort = editedValue
        }
        PropAndValDisplay {
            id:awayIp
            title:"Away Ip Address"
            value: manager.externalip
            onSet: manager.externalip = editedValue
        }

        PropAndValDisplay {
            id:skin
            title:"Skin"
            value: manager.currentSkin
            onSet: manager.currentSkin = editedValue
        }

        PropAndValDisplay {
            id:deviceType
            title:"Type of Device"
            value: manager.isPhone
            onSet: manager.isPhone = editedValue
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
        },
        State {
            name: "editing"
            PropertyChanges {
                target: firstRunOptions
                height:parent.height
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
            PropertyChanges{
                target:optionCol
                visible:true
            }
        }
    ]
}
