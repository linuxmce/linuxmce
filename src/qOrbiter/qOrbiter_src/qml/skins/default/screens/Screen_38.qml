import QtQuick 2.2
import "../."
import "../components"
StyledScreen {
    screen: qsTr("Device Control")
    Panel{
       headerTitle:screen
       content: Item {
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
                    text:qsTr("Please choose device to control. Only active devices are shown.")
                    anchors.centerIn: parent
                    visible:avcodes_root.state==="ready"
                    fontSize:appStyle.appFontSize_title

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
                    margins: appStyle.scaleY(2)
                }

              delegate: StyledButton{
               height: appStyle.listViewItemHeight
               phil: status ? "green" : "blue"
               width: parent.width
               buttonText: qsTr("Device: %1 ").arg(name)
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
                    margins: appStyle.scaleY(2)
                }
                model:deviceCommands
                label: qsTr("Commands")
                delegate: StyledButton{
                    height: appStyle.listViewItemHeight
                    buttonText: name
                    width: parent.width
                    onActivated: {
                     //   console.log("\n"+JSON.stringify(props, null, "\t"))
                        manager.resendCode(selectedDevice, commandnumber)
                    }
                }
            }


            states: [
                State {
                    name: "ready"
                    PropertyChanges {
                        target: avcodes_hdr
                        height:appStyle.scaleY(8)

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





}
