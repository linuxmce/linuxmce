/*!
 *\brief Lighting Floorplan Screen.
 *
 *\ingroup qml_desktop_default
 *
 *This is the screen displaying the lighting floorplan.
 */
import QtQuick 1.1
import "../components"
Rectangle{
    id:lights
    height: manager.appHeight
    width: manager.appWidth
    color:"transparent"

    ListModel{
        id:lightingCommands

        ListElement{
            cmd:"On"
            level:100
        }

        ListElement{
            cmd:"90"
            level:90
        }


        ListElement{
            cmd:"80"
            level:90
        }


        ListElement{
            cmd:"70"
            level:70
        }


        ListElement{
            cmd:"60"
            level:60
        }


        ListElement{
            cmd:"50"
            level:50
        }


        ListElement{
            cmd:"40"
            level:40
        }


        ListElement{
            cmd:"30"
            level:100
        }


        ListElement{
            cmd:"20"
            level:100
        }


        ListElement{
            cmd:"10"
            level:10
        }


        ListElement{
            cmd:"Off"
            level:0
        }


    }


    ListView{
        height: scaleY(65)
        width: scaleX(15)
        anchors.left: parent.left
        anchors.leftMargin: scaleX(2)
        anchors.verticalCenter: parent.verticalCenter
        clip:true
        visible: floorplan_devices.itemSelected ? true : false
        model:lightingCommands
        spacing:5
        delegate: Rectangle{
            height: scaleX(8)
            width: scaleY(8)
            Text {
                height: parent.height
                width: parent.width
                text: cmd
                font.pixelSize: scaleY(3)
                anchors.centerIn: parent
            }

            MouseArea{
                anchors.fill: parent
                onClicked: floorplan_devices.set(level)
            }
        }

    }


FloorPlanDisplay{id: lightingfloorplan; anchors.centerIn: parent}

        HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}
    }


