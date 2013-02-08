import QtQuick 1.0

Component{

Item{
    //important!! these need to be set on an imported component otherwise its appears all wrong!
    height: childrenRect.height
    width: childrenRect.width

ButtonSq
{       id:delegateButton
        height: style.buttonH
        width: style.buttonW
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
                id:buttonimage
                source: "../../../img/icons/"+label+".png"
                height: style.iconHeight
                width: style.iconWidth
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

