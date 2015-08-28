import QtQuick 2.2
import "../."
import "../components"
StyledScreen {
    screen: qsTr("Device Control")

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
                text:qsTr("Please choose device to control")
                anchors.centerIn: parent

                visible:avcodes_root.state==="ready"
                fontSize:Style.scaleY(6)

            }
        }
        GenericListModel {
            id: headerListView
           label: "AV - Devices"
           model:deviceList

            anchors{
                left:parent.left
                right:parent.horizontalCenter
                bottom:parent.bottom
                top:avcodes_hdr.bottom
                margins: Style.scaleY(2)
            }

          delegate: StyledButton{
           height: Style.listViewItemHeight
           phil: status ? "green" : "blue"
           width: parent.width
           buttonText: name+"\n"+"Room:"+location
             onActivated: {
                  deviceCommands.clear()
                //  console.log("\n"+JSON.stringify(props, null, "\t"))
                  // var p = headerListModel[idx]["name"]
                  // console.log("p")
                  manager.showDeviceCodes(devicenumber);avcodes_root.selectedDevice = devicenumber
              }
          }


        }

        GenericListModel{
            id:commandView
            anchors{
                left:parent.horizontalCenter
                right:parent.right
                bottom:parent.bottom
                top:avcodes_hdr.bottom
                margins: Style.scaleY(2)
            }
            model:deviceCommands
            label: qsTr("Commands")
            delegate: StyledButton{
                height: Style.listViewItemHeight
                buttonText: name
                width: parent.width
                onActivated: {
                    console.log("\n"+JSON.stringify(props, null, "\t"))
                    manager.resendCode(selectedDevice, commandnumber)
                }
            }
        }


        states: [
            State {
                name: "ready"
                PropertyChanges {
                    target: avcodes_hdr
                    height:Style.scaleY(8)

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



}
