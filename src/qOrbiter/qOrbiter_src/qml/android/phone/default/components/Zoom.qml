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

            buttonText: "-7%"
            onActivated: manager.setZoomLevel("-7")

        }
        StyledButton{

            buttonText: "-5%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("-5")
            }
        }

        StyledButton{

            buttonText: "-4%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("-4")
            }
        }
        StyledButton{

            buttonText: "-2%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("-2")
            }
        }
        StyledButton{

            buttonText: "-1%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("-1")
            }
        }

        StyledButton{

            buttonText: "+1%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("+1")
            }
        }
        StyledButton{

            buttonText: "+2%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("+2")
            }
        }

        StyledButton{

            buttonText: "+4%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("+4")
            }
        }
        StyledButton{

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

            buttonText: "90%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("90")
            }
        }

        StyledButton{

            buttonText: "95%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("95")
            }
        }
        StyledButton{

            buttonText: "100%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("100")
            }
        }
        StyledButton{

            buttonText: "105%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("105")
            }
        }

        StyledButton{

            buttonText: "110%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("110")
            }
        }
        StyledButton{

            buttonText: "115%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("115")
            }
        }

        StyledButton{

            buttonText: "120%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("120")
            }
        }
        StyledButton{

            buttonText: "125%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("125")
            }
        }
        StyledButton{

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
            buttonText: "Auto"
           textSize:scaleY(8)
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("auto")
            }
        }
        StyledButton{

            buttonText: "1:1"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("1:1")
            }
        }

        StyledButton{

            buttonText: "4:3"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("4:3")
            }
        }

        StyledButton{

            buttonText: "16:9"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("16:9")
            }
        }
        StyledButton{

            buttonText: "2.11:1"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("2.11:1")
            }
        }
    }
}





