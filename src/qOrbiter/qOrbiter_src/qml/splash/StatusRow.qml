import QtQuick 2.3
Item{
    anchors{
        left:parent.left
        right:parent.right
    }

    Rectangle{
        color:"black"
        anchors.fill: parent
        opacity:.85
    }
    Row{
        id:statusRow
        height: scaleX(10)
        anchors{
            left:parent.left
            right:parent.right
        }

        spacing: scaleX(10)

        Rectangle{

            id:connection_indicator
            height: scaleX(5)
            width: scaleX(5)
            color: "transparent"

            Text {
                id: connection_label
                text: qsTr("Connection")
                color: window.b_connectionPresent ? "green" : "red"
                font.pointSize: window.b_connectionPresent ? 14 : 12
//                font.family: myFont.name
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
                color: window.b_devicePresent ? "green" : "red"
                font.pointSize: window.b_devicePresent ? 14 : 12
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
                color: window.b_localConfigReady ? "green" : "red"
                font.pointSize: window.b_localConfigReady ? 14 : 12
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
                color: window.b_skinIndexReady ? "green" : "red"
                font.pointSize: window.b_skinIndexReady ? 14 : 12
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
                color: window.b_orbiterConfigReady ? "green" : "red"
                font.pointSize: window.b_orbiterConfigReady ? 14 : 12
            }
        }


    }
}
