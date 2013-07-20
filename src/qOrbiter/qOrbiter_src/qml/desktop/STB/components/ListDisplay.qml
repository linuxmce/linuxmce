import QtQuick 1.1

    ListView
    {
        property int floorplanType:0
        id:floorplanDevices
        width: scaleX(85)
        height: scaleY(85)
        model:floorplan_devices
        anchors.top: parent.top
        anchors.left: parent.left
        clip:true
        focus:true
        orientation: ListView.Vertical
        spacing:scaleY(1)
        delegate:
            Item{
            id:fp_device_rect
            height: floorplanDevices.currentIndex === index ? scaleY(20) : fpDevice_name.paintedHeight
            width: parent.width
            Component.onCompleted:  manager.getFloorplanDeviceCommands(deviceno);
            Keys.onPressed: {
                if(event.key===Qt.Key_Left){
                    fp_submodel.decrementCurrentIndex()
                } else if(event.key===Qt.Key_Right){
                    fp_submodel.incrementCurrentIndex()
                }
            }

            Rectangle{
                id:phil
                anchors.fill: parent
                color:mickey.pressed ? skinStyle.darkHighlightColor :skinStyle.mainColor
                opacity: .75

            }

            StyledText {
                id: fpDevice_name
                text: name
                fontSize: headerText
                color:"white"
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Column{
                id:infocol
                height: parent.height
                width: parent.width*.25
                anchors.right: parent.right
                anchors.top: parent.top

                Text {
                    id: fpDevice_type
                    text: "I am type" + type
                    color:"white"
                }
                Text {
                    id: fp_type
                    text: "Floorplan type" + floorplantype
                    color:"white"

                }
                Text {
                    id: fpDevice_no
                    text: "I am Dev#" + deviceno
                    color:"white"
                }
                Text {
                    id: fpDevice_pos
                    text: "Position" + floorplan_devices.getDeviceX(deviceno) + "," + floorplan_devices.getDeviceY(deviceno)
                    color:"white"
                    // onTextChanged: placeSprites(floorplan_devices.getDeviceX(deviceno),floorplan_devices.getDeviceY(deviceno),deviceno)
                }
            }

            ListView{
                id:parameters
                height: parent.height
                width: 200
                model:paramlist
                anchors.right: parent.right
                anchors.bottom:parent.bottom
                delegate: StyledText{
                    text: parameters.model[index].CommandDescription
                    fontSize: mediumText
                    color:"white"
                    font.bold: true
                }
            }
            Connections{
                target: parameters
                onCountChanged:console.log("Param Model==>"+JSON.stringify(parameters.model))
            }

            MouseArea{
                id:mickey
                anchors.fill: parent
                onClicked: manager.getFloorplanDeviceCommands(deviceno)
            }

            ListView{
                id:fp_submodel
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                opacity: floorplanDevices.currentIndex === index ? 1 : 0
                width: parent.width
                height: parent.height/2
                model:commandlist["commands"]
                orientation:ListView.Horizontal
                spacing:scaleX(2)
                delegate: Rectangle{
                    id:thisone
                    height:50
                    width: lbl.width
                    radius:5
                    color: fp_submodel.currentIndex === index ? skinStyle.mainColor : sublist_hit.containsMouse ? "lightblue": sublist_hit.pressed ? "green": "darkblue"
                    StyledText{
                        id:lbl
                        text: commandlist["commands"][index].command_name
                        anchors.centerIn: parent
                        fontSize: mediumText
                        font.bold: true
                        color:"white"
                    }
                    MouseArea{
                        id:sublist_hit
                        anchors.fill: thisone
                        onClicked: {console.log("Grabbed"); console.log(JSON.stringify(commandlist["commands"][index])); requestParamManager.getParams(commandlist["commands"][index].command_number, deviceno) }

                    }
                }
            }
        }
    }


