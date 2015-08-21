import QtQuick 1.1
Item{



    Row{
        id:statusRow
        height: childrenRect.height
        width: scaleX(85)
        spacing: scaleX(10)

        Rectangle{

            id:connection_indicator
            height: scaleX(5)
            width: scaleX(5)
            color: "transparent"

            Text {
                id: connection_label
                text: qsTr("Connection")
                color: orbiterWindow.b_connectionPresent ? "green" : "red"
                font.pixelSize: orbiterWindow.b_connectionPresent ? 14 : 12
                font.family: myFont.name
            }
        }

        Rectangle{
            id:device_indicator
            height: scaleX(5)
            width: scaleX(5)
            color: "transparent"
            Image {
                id: device_icon
                source: ""
            }

            Text {
                id: device_Label
                text: qsTr("Device")
                color: orbiterWindow.b_devicePresent ? "green" : "red"
                font.pixelSize: orbiterWindow.b_devicePresent ? 14 : 12
                font.family: myFont.name
            }
        }

        Rectangle{
            id:config_indicator
            height: scaleX(5)
            width: scaleX(5)
            color: "transparent"
            Image {
                id: config_icon
                source: ""
            }
            
            Text {
                id: config_label
                text: qsTr("Config")
                color: orbiterWindow.b_localConfigReady ? "green" : "red"
                font.pixelSize: orbiterWindow.b_localConfigReady ? 14 : 12
            }
        }
        
        Rectangle{
            id:skin_indicator
            height: scaleX(5)
            width: scaleX(5)
            color: "transparent"
            Image {
                id: skin_icon
                source: ""
            }
            
            Text {
                id: skin_label
                text: qsTr("Skins")
                color: orbiterWindow.b_skinIndexReady ? "green" : "red"
                font.pixelSize: orbiterWindow.b_skinIndexReady ? 14 : 12
            }
        }
        
        Rectangle{
            id:skindata_indicator
            height: scaleX(5)
            width: scaleX(5)
            color: "transparent"
            Image {
                id: skinData_icon
                source: ""
            }
            
            Text {
                id: skin_data_label
                text: qsTr("Orbiter Ready")
                color: orbiterWindow.b_orbiterConfigReady ? "green" : "red"
                font.pixelSize: orbiterWindow.b_orbiterConfigReady ? 14 : 12
            }
        }


    }
}
