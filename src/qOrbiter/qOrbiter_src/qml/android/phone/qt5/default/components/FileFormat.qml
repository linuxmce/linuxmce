import QtQuick 2.2

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
           Rectangle{
               id:exit_button
               height: scaleY(8)
               width: parent.width
               anchors.top: rect.top
               color: "transparent"
               Image {
                   id: headerbg
                   source: "../img/widegreyshape.png"
                   anchors.fill: exit_button
               }

               Text {
                   id: exit
                   text: qsTr("Exit")
                   font.pixelSize: scaleY(3)
                   anchors.centerIn: parent
               }
               MouseArea{
                   anchors.fill: parent
                   onClicked: loadComponent("NullComponent.qml")
               }
           }
           Component{
                 id:fileformatdelegate
                 Item {
                     id: fileformatitem
                     height: scaleY(10)
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
                                      onPressAndHold: {
                                          cindex=desc
                                          fileformatmodel.setSelectionStatus(name)
                                          fileformatcell.color = status ? "green" : "red"
                                          console.log(status)
                                          loadComponent("NullComponent.qml")
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




