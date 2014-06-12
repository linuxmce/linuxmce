import QtQuick 1.1

Item{
    id:codelistrect
    anchors.fill: parent


    Rectangle{
        id:labelHeader
        width: parent.width
        height: scaleY(8)
        anchors.top:parent.top
        //   anchors.topMargin: 10
        color:"black"
        StyledText{
            text:"Zoom & Aspect"
            anchors.centerIn: parent
            fontSize: scaleY(8)
        }
    }

    Column{
        id:smallzoom
        width: manager.appWidth * .20

        anchors.top:labelHeader.bottom
        anchors.left:parent.left
        anchors.leftMargin: scaleX(15)
        anchors.bottom:parent.bottom
        spacing:scaleY(2)

        StyledButton{
            state: "fixed"
            buttonText: "-7%"
            onActivated: manager.setZoomLevel("-7")

        }
        StyledButton{
            state: "fixed"
            buttonText: "-5%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("-5")
            }
        }

        StyledButton{
            state: "fixed"
            buttonText: "-4%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("-4")
            }
        }
        StyledButton{
            state: "fixed"
            buttonText: "-2%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("-2")
            }
        }
        StyledButton{
            state: "fixed"
            buttonText: "-1%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("-1")
            }
        }

        StyledButton{
            state: "fixed"
            buttonText: "+1%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("+1")
            }
        }
        StyledButton{
            state: "fixed"
            buttonText: "+2%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("+2")
            }
        }

        StyledButton{
            state: "fixed"
            buttonText: "+4%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("+4")
            }
        }
        StyledButton{
            state: "fixed"
            buttonText: "+5%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("+5")
            }
        }
        StyledButton{

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
            top: labelHeader.bottom
            bottom: parent.bottom
        }
        width: manager.appWidth * .20
        spacing:scaleY(2)

        StyledButton{
            state: "fixed"
            buttonText: "90%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("90")
            }
        }

        StyledButton{
            state: "fixed"
            buttonText: "95%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("95")
            }
        }
        StyledButton{
            state: "fixed"
            buttonText: "100%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("100")
            }
        }
        StyledButton{
            state: "fixed"
            buttonText: "105%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("105")
            }
        }

        StyledButton{
            state: "fixed"
            buttonText: "110%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("110")
            }
        }
        StyledButton{
            state: "fixed"
            buttonText: "115%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("115")
            }
        }

        StyledButton{
            state: "fixed"
            buttonText: "120%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("120")
            }
        }
        StyledButton{
            state: "fixed"
            buttonText: "125%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("125")
            }
        }
        StyledButton{
            state: "fixed"
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
            top: labelHeader.bottom
            bottom: parent.bottom
        }
        width: manager.appWidth * .20
        spacing:scaleY(2)

        StyledButton{
            state: "fixed"
            buttonText: "Auto"

            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("auto")
            }
        }
        StyledButton{
            state: "fixed"
            buttonText: "1:1"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("1:1")
            }
        }

        StyledButton{
            state: "fixed"
            buttonText: "4:3"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("4:3")
            }
        }

        StyledButton{
            state: "fixed"
            buttonText: "16:9"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("16:9")
            }
        }
        StyledButton{
            state: "fixed"
            buttonText: "2.11:1"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("2.11:1")
            }
        }
    }
}





