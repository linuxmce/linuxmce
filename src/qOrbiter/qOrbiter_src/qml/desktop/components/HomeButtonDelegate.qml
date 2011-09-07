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
            height: scaleY(13)
            width: scaleX(8)
            color: style.homescreenscenariobuttoncolor
            radius: style.but_smooth
            buttontext: ""
            buttontextfontsize: 10
            buttontextzindex: 4
            opacity: .5
            MouseArea{
                anchors.fill: parent
                onClicked: execGrp(params);
                z:5    }

            Image {
                id: buttonsqimg
                height:parent.height
               width: parent.width
                source: "../../../img/icons/buttonoverlay.png"
            }


            Image {
                id:buttonimage
                source: "../../../img/icons/"+label+".png"
                height: parent.height
                width: parent.width
                anchors.centerIn: delegateButton
            }

        }

        Text {
            id: daslabel
            text: label
            font.pointSize: 12
            wrapMode: "WrapAtWordBoundaryOrAnywhere"
            anchors.centerIn: delegateButton
            anchors.fill: delegateButton
            z:2
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.family: "Droid Sans"
        }
    }
}

