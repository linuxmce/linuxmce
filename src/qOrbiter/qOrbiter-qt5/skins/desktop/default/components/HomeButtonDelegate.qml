import QtQuick 1.0

Component{

    Item{
        //important!! these need to be set on an imported component otherwise its appears all wrong!

        height:  children.height


        id:lightingdelegate
        Rectangle{
            id:delegatemenu
            width: 200
            height: 25
            border.color: "white"
            border.width: 1
            color:"transparent"

            Text {
                id: item
                text: title
                color: "white"
                font.pointSize: 14
                anchors.centerIn: parent
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    execGrp(params)
                    lightingmenu.destroy()
                }
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

