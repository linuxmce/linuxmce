import QtQuick 2.2

Item {
    anchors.fill: parent
    id:avcodes_root
    property int selectedDevice:-1


    Component.onCompleted: {
        state="loading"
        manager.showAvControl()
        state="ready"
    }

    Item{
        id:avcodes_hdr
        Rectangle{
            id:codefiller
            anchors.fill: parent
            color:"transparent"
        }
        StyledText{
            id:hdrText
            text:"Please choose device to control"
            anchors.centerIn: parent
            visible:avcodes_root.state==="ready"
            fontSize:scaleY(8)
           // isBold: true

    }

    HeaderListView {
        id: headerListView
        isUsingIndex: false
        anchors{
            left:parent.left
            right:parent.horizontalCenter
            bottom:parent.bottom
            top:avcodes_hdr.bottom
            margins: scaleY(2)
        }

        headerListModel: deviceList
        displayProperty: "name"
        listTitle:"AV - Devices"
        returnProperties:["devicenumber", "name", "device_number"]
        onActivationCallback: {
            deviceCommands.clear()
            console.log("\n"+JSON.stringify(props, null, "\t"))
            // var p = headerListModel[idx]["name"]
            // console.log("p")
            manager.showDeviceCodes(props.devicenumber);selectedDevice = props.devicenumber
        }
    }

    HeaderListView{
        id:commandView
        anchors{
            left:parent.horizontalCenter
            right:parent.right
            bottom:parent.bottom
            top:avcodes_hdr.bottom
            margins: scaleY(2)
        }

        headerListModel: deviceCommands
        displayProperty: "name"
        listTitle:"Commands"
        returnProperties: ["commandnumber"]
        onActivationCallback: {
            console.log("\n"+JSON.stringify(props, null, "\t"))
            manager.resendCode(selectedDevice, props.commandnumber)
        }
    }


    states: [
        State {
            name: "ready"
            PropertyChanges {
                target: avcodes_hdr
                height:scaleY(8)

            }
            PropertyChanges {
                target: codefiller
                color:"green"
            }

            AnchorChanges{
                target: avcodes_hdr
                anchors{
                    top:avcodes_root.top
                    left:avcodes_root.left
                    right:avcodes_root.right
                }
            }
        },
        State{
            name:"loading"
            PropertyChanges {
                target: avcodes_hdr
                height:scaleY(2)

            }
            PropertyChanges {
                target: codefiller
                color:"yellow"
            }
            AnchorChanges{
                target: avcodes_hdr
                anchors{
                    top:avcodes_root.top
                    left:avcodes_root.left
                    right:avcodes_root.right
                }
            }
        }

    ]
}
