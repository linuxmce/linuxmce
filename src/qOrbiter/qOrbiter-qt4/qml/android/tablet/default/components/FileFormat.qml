import QtQuick 1.0

Rectangle {
           id:rect
           width: scaleX(35)
           height: scaleY(65)
           color: "transparent"
           anchors.centerIn: parent
           border.color: style.button_system_color
           border.width: 1
           clip: true
           property string cindex
           focus: true
           MouseArea{
               anchors.fill: parent

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
                                      loadComponent("NullComponent.qml")
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
                                 onFocusChanged: {loadComponent("NullComponent.qml")}
                                 font.pixelSize: scaleY(2)

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


            }

        }




