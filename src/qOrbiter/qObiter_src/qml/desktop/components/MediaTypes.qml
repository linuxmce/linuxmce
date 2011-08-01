import QtQuick 1.0

Rectangle {
           id:rect
           width: 200
           height: 200
           color: style.bgcolor
           anchors.centerIn: parent
           border.color: style.button_system_color
           border.width: 1
           clip: true


         Component{
               id:mediatypedelegate
               Item {
                   id:mediatypeitem
                   height: 50
                   width: parent.width

                   Rectangle{
                       id: mediatyperect
                       height: 50
                       width: parent.width
                       border.color: "white"
                       border.width: 1
                       color: status ? "green" : style.alert_bg

                       MouseArea{
                           anchors.fill: parent
                           onPressAndHold: {rect.destroy()
                           }

                           onClicked:{
                               mediatypemodel.setSelectionStatus(name)

                                     }
                       }
                       Text {
                           id: mediatypecell
                           text: name
                           font.pointSize: 14
                       }


                   }
               }
           }


        ListView{
            id:mediatypelist
            height: parent.height
            width: parent.width
            model: mediatypemodel
            delegate: mediatypedelegate

        }

   }


