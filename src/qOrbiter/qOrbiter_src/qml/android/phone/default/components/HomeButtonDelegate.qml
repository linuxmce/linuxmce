import QtQuick 1.0
Component{

    Item{
        id:middle
        //important!! these need to be set on an imported component otherwise its appears all wrong!
        height:  delegatemenu.height
        width: delegatemenu.width

        Rectangle{
            id:delegatemenu
            width: 200
            height: 55
            border.color: "white"
            border.width: 1
            color:"transparent"

            Text {
                id: generic_label
                text: title
                color: "white"
                font.pixelSize: 11
                anchors.centerIn: parent
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    execGrp(params);
                    genericlist.destroy()
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

