import QtQuick 2.0

Rectangle {
           id:rect
           width: scaleX(20)
           height: scaleY(30)
           color: "transparent"
           anchors.centerIn: parent
           border.color: style.button_system_color
           border.width: 1
           clip: true

           ListModel{
               id:fileformatmodel

               ListElement{
                   name:"Action"
                   isChosen:false
                   pk: 1

               }

               ListElement{
                   name:"Adventure"
                   isChosen:false
                   pk: 2

               }

               ListElement{
                   name:"Drama"
                   isChosen:false
                   pk: 3

               }

               ListElement{
                   name:"Comedy"
                   isChosen:false
                   pk: 4

               }


           }

           Component{
               id:genrefilterdelegate
               Item {
                   id: genrefilteritem
                   height: 50
                   width: parent.width
                   MouseArea{
                       anchors.fill: parent
                       onClicked:{

                           setStringParam(3, pk)
                                   rect.destroy()
                       }
                   }


                   Rectangle{
                       height: 50
                       width: parent.width
                       color: style.alert_bg
                       border.color: "white"
                       border.width: 1

                       StyledText {
                           id: genreselectcell
                           text: name
                           font.pointSize: 14
                             }

                       Rectangle{
                           height: 20
                           width: 20
                           color: status ? "red" : "green"
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

        }

   }


