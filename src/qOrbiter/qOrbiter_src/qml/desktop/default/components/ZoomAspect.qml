import QtQuick 1.1

Rectangle {
    id:zoomaspectratio
    width: scaleX(50)
    height: scaleY(50)
    radius: 10
    border.color: "aliceblue"
    border.width: 2
    color: "slategrey"
    anchors.centerIn: parent

    Rectangle
    {
        id:codelistrect
        height: scaleY(45)
        width: scaleX(45)
        anchors.centerIn: parent
        border.color: "black"
        border.width: 2
        color: "aliceblue"

        Row{
            id:containerrow
            height: childrenRect.height
            width: childrenRect.width
            anchors.centerIn: parent

            Column{
                id:smallzoom

                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "-7%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("-7")
                    }
                }
                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "-5%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("-5")
                    }
                }

                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "-4%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("-4")
                    }
                }
                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "-2%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("-2")
                    }
                }
                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "-1%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("-1")
                    }
                }

                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "+1%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("+1")
                    }
                }
                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "+2%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("+2")
                    }
                }

                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "+4%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("+4")
                    }
                }
                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "+5%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("+5")
                    }
                }
                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "+7%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("+7")
                    }
                }

            }

            Column{
                id:largezoom

                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "90%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("90")
                    }
                }

                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "95%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("95")
                    }
                }
                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "100%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("100")
                    }
                }
                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "105%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("105")
                    }
                }

                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "110%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("110")
                    }
                }
                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "115%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("115")
                    }
                }

                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "120%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("120")
                    }
                }
                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "125%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("125")
                    }
                }
                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "130%"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setZoomLevel("130")
                    }
                }

            }

            Column{
                id:aspect
                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "Auto"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setAspectRatio("auto")
                    }
                }
                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "1:1"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setAspectRatio("1:1")
                    }
                }

                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "4:3"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setAspectRatio("4:3")
                    }
                }

                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "16:9"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setAspectRatio("16:9")
                    }
                }
                ButtonSq{
                    height: scaleY(4)
                    width: scaleX(9)
                    buttontext: "2.11:1"
                    MouseArea{
                        anchors.fill: parent
                        onClicked:manager.setAspectRatio("2.11:1")
                    }
                }

            }

            ButtonSq
            {
                height: scaleY(5)
                width: scaleX(5)
                buttontext: "Close"
                buttonsqradius: 10
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                MouseArea{
                    anchors.fill: parent
                    onClicked: zoomaspectratio.visible = false
                }
            }
        }

    }

}
