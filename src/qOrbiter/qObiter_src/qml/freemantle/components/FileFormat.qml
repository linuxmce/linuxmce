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

           ListModel{
               id:fileformatmodel

               ListElement{
                   name:"Low Resolution"
                   pk: 1

               }

               ListElement{
                   name:"DVD"
                   pk: 2

               }

               ListElement{
                   name:"Standard Definition"
                   pk: 3

               }

               ListElement{
                   name:"HD 720"
                   pk: 4

               }

               ListElement{
                   name:"HD 1080"
                   pk: 5

               }

               ListElement{
                   name:"Low Quality"
                   pk: 6

               }

               ListElement{
                   name:"MP3"
                   pk: 7

               }

               ListElement{
                   name:"CD Quality"
                   pk: 8

               }
               ListElement{
                   name:"High Def audio"
                   pk: 9

               }

               ListElement{
                   name:"Rom"
                   pk: 10

               }

               ListElement{
                   name:"Disc"
                   pk: 11

               }
           }

           Component{
               id:fileformatdelegate
               Item {
                   id: fileformatitem
                   height: 50
                   width: parent.width


                   Rectangle{
                       height: 50
                       width: parent.width
                       color: style.alert_bg
                       border.color: "white"
                       border.width: 1

                       Text {
                           id: fileformatcell
                           text: name
                           font.pointSize: 14

                       }
                   }
               }
           }


        ListView{
            id:mediatypeslist
            height: parent.height
            width: parent.width
            model: fileformatmodel
            delegate: fileformatdelegate
            MouseArea{
                anchors.fill:parent
                onClicked: rect.destroy()

               }
        }

   }

