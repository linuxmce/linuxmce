import QtQuick 1.1
Rectangle{
    id:power_rect_container
    height: appH
    width: appW
    color: "transparent"
    MouseArea{
        anchors.fill: parent
    }


    Rectangle {
        id:powerrect
        width: scaleX(40)
        height: scaleY(40)
        clip: true
        color: style.darkhighlight
        anchors.centerIn: parent
        border.color: style.lighthighlight
        border.width: 2
        radius: 5
        scale: 0
        opacity: 0
        x: 0
        y: 0

        MouseArea{
            anchors.fill: parent

        }

        Component.onCompleted: show.running = true

        ParallelAnimation{
            id: show
            running:false
            PropertyAnimation { target: powerrect; property: "opacity"; to: 1; duration: 750}
            PropertyAnimation { target: powerrect; property: "scale"; to: 1; duration: 750}
            PropertyAnimation { target: powerrect; property: "x"; easing.type: Easing.InElastic; to: appW/2; duration: 750}
            PropertyAnimation { target: powerrect; property: "y"; easing.type: Easing.InElastic; to: appH/2; duration: 750}
        }

        ParallelAnimation{
            id: hide
            running:false
            PropertyAnimation { target: powerrect; property: "opacity"; to: 0; duration: 750}
            PropertyAnimation { target: powerrect; property: "scale"; to: 0; duration: 750}
            PropertyAnimation { target: powerrect; property: "x"; to: appW; duration: 750}
            PropertyAnimation { target: powerrect; property: "y"; to: appH; duration: 750}
            ScriptAction {script:power_rect_container.destroy()}
        }

        Rectangle{

            width: powerrect.width-1
            height: powerrect.height-1
            color:"transparent"
            clip:true
            anchors.centerIn: parent
            Rectangle{
                id:exit_button
                height: scaleY(10)
                width: parent.width
                radius: parent.radius
                color: "grey"

                Text {
                    id: exit_label
                    text: qsTr("Close")
                    anchors.centerIn: parent
                    font.pixelSize: scaleY(3)
                    font.bold: true
                }
                MouseArea{
                    anchors.fill: parent
                    onClicked: hide.running = true
                }
            }


            Row{
                anchors.centerIn: parent
                width: childrenRect.width
                height: childrenRect.height
                spacing: scaleX(5)
                Column {
                    id: display_on
                    width: childrenRect.width
                    height: childrenRect.height
                    spacing: 10
                    ButtonSq{
                        id:pwron
                        buttontext: qsTr("Display On")
                        buttonsqradius: 20
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                manager.displayToggle(1)
                                power_rect_container.destroy()
                            }
                        }
                    }

                }

                Column {
                    id: display_off
                    width: childrenRect.width
                    height: childrenRect.height
                    spacing: 10
                    ButtonSq {
                        id: offlabel
                        buttontext: qsTr("Display Off")
                        buttonsqradius: 20
                        MouseArea{
                            anchors.fill: parent
                            onClicked: {
                                manager.displayToggle(0)
                                 power_rect_container.destroy()
                            }
                        }
                    }
                }
            }
        }
    }
}
