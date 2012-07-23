import QtQuick 1.0
Component{

    Item{
        id:middle
        //important!! these need to be set on an imported component otherwise its appears all wrong!
        height:  delegatemenu.height
        width: delegatemenu.width

        Rectangle{
            id:delegatemenu
            width: scaleX(61)
            height: scaleY(12)
            border.color: "orange"
            border.width: 1
            color:"white"


            Text {
                id: generic_label
                text: title
                color: "black"
                font.pixelSize: scaleY(3)
                anchors.left: delegatemenu.left
            }

            Image {
                id: selectionIndicator
                source: delegateHit.pressed ? "../img/radio_btn_active.png" : "../img/radio_btn_inactive.png"
                height: 45
                width: 45
                anchors.right: delegatemenu.right
            }

            MouseArea{
                id:delegateHit
                anchors.fill: parent
                onClicked: {
                    dcerouter.executeCommandGroup(params);
                    loadComponent("NullComponent.qml")
                }
                onPressed:  delegatemenu.color = "orange"
                onReleased: delegatemenu.color = "transparent"
            }
        }
    }
    /*
        ImgButton
        {       id:delegateButton
            anchors.top: parent.top
            anchors.topMargin: scaleY(2)
            color:"transparent"
            // height: style.stdbuttonh
            // width: style.stdbuttonw
            buttontext: title
            buttontextbold: true
            buttontextitalic: true
            buttonopacity: .85

            MouseArea{
                anchors.fill: delegateButton
                onClicked: execGrp(params);
                hoverEnabled: true

                onEntered: {
                    delegateButton.buttontextcolor = "white"
                    delegateButton.buttonopacity = 1

                }
                onExited: {
                    delegateButton.buttontextcolor = "black"
                    delegateButton.buttonopacity = .85

                }
            }

        }
        /*
        Text {
            id: daslabel
            text: label
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            anchors.centerIn: delegateButton
            anchors.fill: delegateButton
            z:2
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.family: "Droid Sans"
        }
        */

}

