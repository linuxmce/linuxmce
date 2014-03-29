import QtQuick 2.0

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

            label: "-7%"
            onActivated: manager.setZoomLevel("-7")

        }
        StyledButton{

            label: "-5%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("-5")
            }
        }

        StyledButton{

            label: "-4%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("-4")
            }
        }
        StyledButton{

            label: "-2%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("-2")
            }
        }
        StyledButton{

            label: "-1%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("-1")
            }
        }

        StyledButton{

            label: "+1%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("+1")
            }
        }
        StyledButton{

            label: "+2%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("+2")
            }
        }

        StyledButton{

            label: "+4%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("+4")
            }
        }
        StyledButton{

            label: "+5%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("+5")
            }
        }
        StyledButton{

            label: "+7%"
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

            label: "90%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("90")
            }
        }

        StyledButton{

            label: "95%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("95")
            }
        }
        StyledButton{

            label: "100%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("100")
            }
        }
        StyledButton{

            label: "105%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("105")
            }
        }

        StyledButton{

            label: "110%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("110")
            }
        }
        StyledButton{

            label: "115%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("115")
            }
        }

        StyledButton{

            label: "120%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("120")
            }
        }
        StyledButton{

            label: "125%"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setZoomLevel("125")
            }
        }
        StyledButton{

            label: "130%"
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
            label: "Auto"
           textSize:scaleY(8)
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("auto")
            }
        }
        StyledButton{

            label: "1:1"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("1:1")
            }
        }

        StyledButton{

            label: "4:3"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("4:3")
            }
        }

        StyledButton{

            label: "16:9"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("16:9")
            }
        }
        StyledButton{

            label: "2.11:1"
            MouseArea{
                anchors.fill: parent
                onClicked:manager.setAspectRatio("2.11:1")
            }
        }
    }
}





