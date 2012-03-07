import QtQuick 1.0
import "../components"

Rectangle {
    id:myth_recordings
    width: appW
    height: appH

    Image {
        id: bg
        source: "../img/bkg.png"
        anchors.fill: parent
    }

    HomeButton{
        id:home
    }

    Text {
        id: temp_label
        text: qsTr("mythTV recordings")
    }

    Rectangle{
        id:buttons_row
        height: scaleY(25)
        width:scaleX(85)
        anchors.centerIn: parent
        color: "transparent"

        Flow{
            id:button_array
            width: parent.width
            height: scaleY(85)

            AvOptionButton{
                buttontext: qsTr("Live")
                MouseArea{
                    anchors.fill: parent
                    onClicked: dcerouter.extraButtons("livetv")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Schedule")
                MouseArea{
                    anchors.fill: parent
                    onClicked: dcerouter.extraButtons("schedule")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Recordings")
                MouseArea{
                    anchors.fill: parent
                    onClicked: dcerouter.extraButtons("recordings")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Music")
                MouseArea{
                    anchors.fill: parent
                    onClicked: dcerouter.extraButtons("music")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Guide")
                MouseArea{
                    anchors.fill: parent
                    onClicked: dcerouter.extraButtons("guide")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Menu")
                MouseArea{
                    anchors.fill: parent
                    onClicked: dcerouter.extraButtons("menu")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Info")
                MouseArea{
                    anchors.fill: parent
                    onClicked: dcerouter.extraButtons("info")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Exit")
                MouseArea{
                    anchors.fill: parent
                    onClicked: dcerouter.extraButtons("exit")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Favorites")
                MouseArea{
                    anchors.fill: parent
                    onClicked: dcerouter.extraButtons("favorites")
                }
            }
            AvOptionButton{
                buttontext: qsTr("Record")
                MouseArea{
                    anchors.fill: parent
                    onClicked: dcerouter.extraButtons("record")
                }
            }
        }



    }

    AudioRemote{
        id:recordings_control
        anchors.bottom: parent.bottom
    }
}
