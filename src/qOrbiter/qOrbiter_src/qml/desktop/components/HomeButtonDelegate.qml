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

            radius: style.but_smooth
            buttontext: label

            MouseArea{
                anchors.fill: parent
                onClicked: execGrp(params);
                z:5    }

            Image {
                id: buttonsqimg
                height:parent.height
               width: parent.width
                source: "../../../img/icons/buttonoverlay.png"
                opacity: .5
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

