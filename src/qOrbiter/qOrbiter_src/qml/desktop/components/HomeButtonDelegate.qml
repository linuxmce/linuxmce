import QtQuick 1.0

Component{

    Item{
        //important!! these need to be set on an imported component otherwise its appears all wrong!
        height: scaleY(15)
        width: scaleX(8)

        ImgButton
        {       id:delegateButton
            anchors.top: parent.top
            anchors.topMargin: scaleY(2)
            height: style.stdbuttonh
            width: style.stdbuttonw
            buttontext: title
            buttontextbold: true
            buttontextitalic: true

            MouseArea{
                anchors.fill: delegateButton
                onClicked: execGrp(params);
                hoverEnabled: true

                onEntered: {
                    delegateButton.buttonbordercolor.color = "white"

                }
                onExited: {
                    delegateButton.buttonbordercolor.color = style.button_action_color_hover

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
}

