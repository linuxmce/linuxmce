import QtQuick 2.0

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


                             StyledText {
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
                                 color: status ? "green" : "red"
                                 z:10
                                  MouseArea{
                                      anchors.fill: parent
                                      onClicked:{
                                          cindex=desc
                                          fileformatmodel.setSelectionStatus(name)
                                          fileformatcell.color = status ? "green" : "red"
                                          console.log(status)
                                                }
                                          }
                                      }

                               }


                         }
                     }
                 }



        ListView{
            id:fileformatlist
            height: parent.height
            width: parent.width
            model: fileformatmodel
            delegate: fileformatdelegate
            focus:true


            }

        }




