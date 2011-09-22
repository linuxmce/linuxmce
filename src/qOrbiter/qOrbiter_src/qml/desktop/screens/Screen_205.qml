import QtQuick 1.0
import "../components"

    Rectangle{
        id:requestingregen
        height: style.orbiterH
        width: style.orbiterW
       // color: style.highlight2
        gradient: Gradient {
            GradientStop {
                position: 0.00;
                color: style.highlight2;
            }

            GradientStop {
                position: 1.00;
                color: style.maincolor;
            }
        }


        Rectangle{
            id:regenrequestcontainer
            height: scaleY(55)
            width:scaleX(50)
            anchors.centerIn: parent
            color: style.darkhighlight
            border.color: style.highlight1
            border.width: 2
            radius: 10


            Column{
                id:buttoncolumn
                height: childrenRect.height
                width: parent.width
                spacing: 5
               anchors.centerIn: parent

                Text {
                    id: name
                    text: qsTr("You have recently changed scenarios, floorplans, or other settings. I need to regenerate the graphical screens so you can see the new buttons. This will take from 2 - 20 minutes")
                    wrapMode: "WrapAtWordBoundaryOrAnywhere"
                    width: parent.width * .85
                    font.pixelSize: scaleY(2.5)
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: "aliceblue"
                }

                ButtonSq{
                    id:thisorbiter
                    height: scaleY(10)
                    width:scaleX(10)
                    buttontext: "Yes Regenerate this orbiter."
                    buttonsqradius: 10
                    anchors.horizontalCenter: parent.horizontalCenter
                }

               ButtonSq{
                   id:allorbiters
                   height: scaleY(10)
                   width:scaleX(10)
                   buttontext: "Regenerate all orbiters in my home."
                   buttonsqradius: 10
                   anchors.horizontalCenter: parent.horizontalCenter
               }

               ButtonSq{
                   id:deny
                   height: scaleY(10)
                   width:scaleX(10)
                   buttontext: "No I will do it later."
                   buttonsqradius: 10
                   anchors.horizontalCenter: parent.horizontalCenter
                   MouseArea{
                       anchors.fill: parent
                       onClicked: {gotoQScreen("Screen_1.qml")}
                   }
               }

               ButtonSq{
                   id:denyall
                   height: scaleY(10)
                   width:scaleX(10)
                   buttontext: "No for all orbiters."
                   buttonsqradius: 10
                   anchors.horizontalCenter: parent.horizontalCenter
                   MouseArea{
                       anchors.fill: parent
                       onClicked: {gotoQScreen("Screen_1.qml")}
                   }
               }
            }
        }       
    }

