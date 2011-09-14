import QtQuick 1.0

Component{

    Item{
        //important!! these need to be set on an imported component otherwise its appears all wrong!
        height: scaleY(15)
        width: scaleX(8)

        ButtonSq
        {       id:delegateButton
            anchors.top: parent.top
            anchors.topMargin: scaleY(2)
            height: style.stdbuttonh
            width: style.stdbuttonw
            buttontextcolor: style.accentcolor
            radius: style.but_smooth
            buttontext: label
            buttontextbold: true
            buttontextitalic: true

            MouseArea{
                anchors.fill: parent
                onClicked: execGrp(params);
                onEntered: delegateButton.buttontextcolor = style.accentcolor
                z:5    }

            Image {
                id: buttonsqimg
                height:parent.height
                fillMode: Image.PreserveAspectCrop
                width: parent.width
                source: "../../../img/icons/buttonoverlay2.png"
                opacity: .85


            }


            Image {
                id:buttonimage
                source: ""
                height: parent.height
                width: parent.width
                anchors.centerIn: delegateButton
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

