import QtQuick 1.0

Item {
    signal showDatagrid( string msg)
    signal gotoScreen( string screen, string type)
    property alias nowplayingtext:now_playing.buttontext
    property alias rowlabel: rowheader.buttontext

    height: style.homescreenrowheight
    width: parent.width

Component
{
    id:mediaDelegate
    Item {
        id: listDelegate
        width: listView.width
        height: childrenRect.height


    ButtonSq
    {       id:buttondelegate
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: label
            buttontextfontsize: 12
            buttontextbold: true
            buttontextzindex: 30

             MouseArea{
                 anchors.fill: parent
                 onClicked: dceObject.execGrp(params);
                        }

              Image {
                    id:buttonimage
                    source: "../../../img/icons/"+label+".png"
                    height: style.iconHeight
                    width: style.iconWidth
                     anchors.centerIn: parent
                     }
                  }
         }
   }


Flickable{
    id:mediaflick
    interactive: true
    height: 80
    width: parent.width
    contentHeight: 80
    contentWidth: childrenRect * 1
    clip: true

    Row {
        id: guide
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        spacing:5


        ButtonSq {
            id: rowheader
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: ""

            Image {
                id: mHeaderImg
                source: "../../../img/icons/kmix.png"
                height: style.iconHeight
                width: style.iconWidth
                anchors.centerIn: parent
                }
            }

        ButtonSq {
            id: now_playing
            height: style.buttonH
            width: style.buttonW
            color: style.button_action_color
            radius: style.but_smooth
            buttontext: ""

            MouseArea{
                anchors.fill: parent
                onClicked: dceObject.gotoQScreen("Screen_48.qml")
            }
            }
        ListView{
            id: listView
            width: stage.width
            height: 50
            model: currentRoomMedia

            orientation:ListView.Horizontal
            spacing: 5
            delegate:
                ButtonSq
                    {
                    id:buttondelegate
                    height: style.buttonH
                    width: style.buttonW
                    color: style.button_action_color
                    radius: style.but_smooth
                    buttontext: label
                    buttontextfontsize: 12
                    buttontextbold: true
                    buttontextzindex: 30

                     MouseArea{
                         anchors.fill: parent
                         onClicked: dceObject.execGrp(params);
                                }

                      Image {
                            id:buttonimage
                            source: "../../../img/icons/"+label+".png"
                            height: style.iconHeight
                            width: style.iconWidth
                             anchors.centerIn: parent
                             }
                         }

              }
         }
    }
}
