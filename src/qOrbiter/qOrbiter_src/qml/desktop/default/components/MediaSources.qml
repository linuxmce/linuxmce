import QtQuick 1.1

Rectangle {
           id:rect
           width: 200
           height: 200
           color: "transparent"
           anchors.centerIn: parent
           border.color: style.button_system_color
           border.width: 1
           clip: true
           property string cindex

           ListModel{
               id:mediasourcelist

               ListElement{
                   name:"Hard Drives"
                   pk:"1"
                   isSelected: false
               }

               ListElement{
                   name:"Discs & Jukeboxes"
                   pk:"2"
                   isSelected: false
               }

               ListElement{
                   name:"Bookmarks"
                   pk:"3"
                   isSelected: false
               }

               ListElement{
                   name:"Downloadable"
                   pk:"4"
                   isSelected: false
               }
           }

           Component{
                 id:fileformatdelegate
                 Item {
                     id: fileformatitem
                     height: 50
                     width: parent.width

                     Rectangle{
                         id: formatrect
                         height: 50
                         width: parent.width
                         border.color: "black"
                         border.width: 1
                         color: "transparent"

                         MouseArea{
                                   anchors.fill: parent
                                   z:0
                                   onClicked:
                                   {
                                       rect.destroy()
                                   }
                              }


                         Row{
                             height: childrenRect.height
                             width: childrenRect.width
                             spacing: 85


                             Text {
                                 width: 75
                                 height: parent.height
                                 id: fileformatcell
                                 text: name
                                 font.pointSize: 14
                                 onFocusChanged: {rect.destroy()}

                                  }

                             Rectangle{
                                 height: 25
                                 width: 25
                                 border.color: "black"
                                 border.width: 1
                                 color: isSelected ? "green" : "red"
                                 z:10
                                  MouseArea{
                                      anchors.fill: parent
                                      onClicked:{

                                          mediasourcelist.setProperty(index, "isSelected", !mediasourcelist.get(index).isSelected)
                                          fileformatcell.color = isSelected ? "green" : "red"

                                                }
                                          }
                                      }

                               }


                         }
                     }
                 }



        ListView{
            id:mediasources
            height: parent.height
            width: parent.width
            model: mediasourcelist
            delegate: fileformatdelegate
            focus:true


            }

        }




