import QtQuick 1.1

Rectangle{
    id:codelistrect
    anchors.fill: parent

    border.width: 2
    color: "black"

    Column{
        id:smallzoom
        width: manager.appWidth * .20

        anchors.top:parent.top
        anchors.left:parent.left
        anchors.leftMargin: scaleX(15)
        anchors.bottom:parent.bottom

        spacing:scaleY(2)


        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "-7%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("-7")
            }
        }
        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "-5%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("-5")
            }
        }

        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "-4%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("-4")
            }
        }
        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "-2%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("-2")
            }
        }
        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "-1%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("-1")
            }
        }

        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "+1%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("+1")
            }
        }
        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "+2%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("+2")
            }
        }

        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "+4%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("+4")
            }
        }
        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "+5%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("+5")
            }
        }
        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "+7%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("+7")
            }
        }
    }

    Column{
        id:largezoom
        anchors{
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            bottom: parent.bottom
        }
        width: manager.appWidth * .20
        spacing:scaleY(2)

        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "90%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("90")
            }
        }

        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "95%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("95")
            }
        }
        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "100%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("100")
            }
        }
        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "105%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("105")
            }
        }

        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "110%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("110")
            }
        }
        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "115%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("115")
            }
        }

        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "120%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("120")
            }
        }
        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "125%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("125")
            }
        }
        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "130%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("130")
            }
        }
    }


    Column{
        id:aspect
        anchors{
            right:parent.right
            rightMargin: scaleX(15)
            top: parent.top
            bottom: parent.bottom
        }
        width: manager.appWidth * .20
        spacing:scaleY(2)

        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "Auto"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("auto")
            }
        }
        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "1:1"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("1:1")
            }
        }

        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "4:3"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("4:3")
            }
        }

        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "16:9"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("16:9")
            }
        }
        StyledButton{
            height: scaleY(4)
            width: scaleX(9)
            buttonText: "2.11:1"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("2.11:1")
            }
        }
    }
}





